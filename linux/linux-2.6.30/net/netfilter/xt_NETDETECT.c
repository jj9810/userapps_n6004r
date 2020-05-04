#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/proc_fs.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/inetdevice.h>
#include <net/checksum.h>

#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_NETDETECT.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("EFM");
MODULE_ALIAS("ipt_NETDETECT");


#define TCP_VIRUS_PORTS_NUM 17
#define UDP_VIRUS_PORTS_NUM 3
static unsigned short TCP_VIRUS_PORTS[TCP_VIRUS_PORTS_NUM] = { 
	80, 135, 139, 445, 707,
	1080, 2535, 2745, 3127, 4751,
	5000, 6667, 6777, 9900, 14247,
	17771, 63535 
};
static unsigned short UDP_VIRUS_PORTS[UDP_VIRUS_PORTS_NUM] = { 137, 1434, 6667 };

struct net_detect_history ND_history[MAX_HISTORY];
struct net_detect_history NDip_history[255];
u32  ND_limit_burst = 0;

static u32  burst_drop_flag = 0;
static u32  virus_drop_flag = 0;


/****************** START : Functions relative to PROC File ******************/
static int proc_read_burst_drop(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        p +=  sprintf(p, "%d %d", burst_drop_flag, virus_drop_flag);

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}

static int proc_write_burst_drop(struct file *file, const char *buffer, u_long count, void *data)
{
        if(buffer[0] == '0' )
                burst_drop_flag= 0;
        else
                burst_drop_flag= 1;

        if(buffer[2] == '0' )
                virus_drop_flag= 0;
        else
                virus_drop_flag= 1;

        return count;
}

static u32 init_net_detect_proc(void)
{
	struct proc_dir_entry *proc_entry, *parent_entry;

	parent_entry = proc_mkdir( "netdetect", 0 );

	proc_entry = create_proc_entry("burst_drop", 0, parent_entry);
	if( proc_entry != NULL )
	{
		proc_entry->read_proc = &proc_read_burst_drop;
		proc_entry->write_proc = &proc_write_burst_drop;
	}

	return 0;
}
/****************** END :   Functions relative to PROC File ******************/

static int virus_drop_only(unsigned char proto, unsigned short port)
{
	int rc = 0, i;

	if (!virus_drop_flag)
	{
		rc = 0;
	}
	else
	{
		if (proto == IPPROTO_TCP)
		{
			for (i=0; i<TCP_VIRUS_PORTS_NUM; i++)
			{
				if (port == TCP_VIRUS_PORTS[i])
				{
					rc = 1;
					break;
				}
			}
		}
		else if (proto == IPPROTO_UDP)
		{
			for (i=0; i<UDP_VIRUS_PORTS_NUM; i++)
			{
				if (port == UDP_VIRUS_PORTS[i])
				{
					rc = 1;
					break;
				}
			}
		}
		else
		{
			rc = 0;
		}
	}


	return rc;
}

static struct net_detect_history *search_net_detect_history(u32 srcip, u8 proto, u16 port)
{
	struct net_detect_history *ndh = NULL;
	int idx;

	for (idx = 0; idx < MAX_HISTORY; idx++)
	{
		if (ND_history[idx].ipaddr == srcip &&
		    ND_history[idx].proto == proto &&
		    ND_history[idx].port == port)
		{
		        ndh = &ND_history[idx];
			break;
		}
	}

	if (ndh == NULL)
	{
		for (idx = 0; idx < MAX_HISTORY; idx++)
		{
			if (ND_history[idx].ipaddr == 0)
				ndh = &ND_history[idx];
		}
	}

	return ndh;
}

extern int check_internal_subnet(u32 ipaddr);

int update_net_detect_history(u32 srcip, u8 proto, u16 port)
{
	struct net_detect_history *ndh;
	unsigned long now = jiffies;
	struct timespec value;
	int ip_index, rc;

	ndh = search_net_detect_history(srcip, proto, port);

	if (ndh == NULL)
	{
		//printk ("slot full \n");
		return 0;
	}

	if (ndh->ipaddr == 0) /* New Detection */
	{
		//printk ("new detect \n");
		ndh->ipaddr = srcip;
		ndh->proto = proto;
		ndh->port = port;
	}
	ndh->pcount++;  /* this variable will be cleared by 1 second period */

	jiffies_to_timespec(now, &value);
	ndh->etime = value.tv_sec;

	ip_index = ntohl(srcip) & 0xff;

	if (burst_drop_flag && 
	    ((ndh->pcount >= ND_limit_burst) || (NDip_history[ip_index].pcount >= (ND_limit_burst * 2))))
		rc = NF_DROP;
	else if (virus_drop_only(ndh->proto, ndh->port) && ndh->pcount >= ND_limit_burst)
		rc = NF_DROP;
	else if (virus_drop_flag && NDip_history[ip_index].pcount >= (ND_limit_burst * 2))
		rc = NF_DROP;
	else
		rc = XT_CONTINUE;

	if (check_internal_subnet(srcip))
	{
		//printk ("local ip %08x \n", srcip);	
		NDip_history[ip_index].ipaddr = srcip;
		if (ndh->pcount >= ND_limit_burst)
			NDip_history[ip_index].pcount = 0;
		else
		{
			NDip_history[ip_index].etime = value.tv_sec;
			NDip_history[ip_index].pcount++;
		}
	}
#if 0
	else
		printk ("Not local ip %08x \n", srcip);	
#endif

	return rc;
}


static unsigned int
target(struct sk_buff *skb, const struct xt_target_param *par)
{
	struct iphdr *iph = ip_hdr(skb);
	struct udphdr *udph = NULL;
	struct tcphdr *tcph = NULL;
	struct icmphdr *icmph = NULL;
	unsigned short dest_port = 0;
	unsigned int srcip = 0;
	unsigned int rc = XT_CONTINUE;
	const struct xt_net_detect_target_info *ndinfo = par->targinfo;

	srcip = iph->saddr;

	if (iph->protocol == IPPROTO_TCP)
	{
		tcph = (void *)iph + iph->ihl*4;
		dest_port = ntohs(tcph->dest);
	}  
	else if (iph->protocol == IPPROTO_UDP)
	{
		udph = (void *)iph + iph->ihl*4; 
		dest_port = ntohs(udph->dest);
	}
	else if (iph->protocol == IPPROTO_ICMP)
	{
		icmph = (void *)iph + iph->ihl*4;
		if (icmph->type != 8) return XT_CONTINUE;
	}

	ND_limit_burst = ndinfo->limit;

	rc = update_net_detect_history(srcip, iph->protocol, dest_port);

	return rc;
}

static bool
checkentry(const struct xt_tgchk_param *par)
{
        if (strcmp(par->table, "filter") != 0) 
	{
		printk("NETDETECT: can only be called from \"filter\" table, not \"%s\"\n", 
	                                par->table); 
                return false;
        }
        return true;
}

static struct xt_target netdetect = {
	.name     	= "NETDETECT",
	.family    	= AF_INET,
	.revision    	= 0,
	.checkentry   	= checkentry,
	.target      	= target,
	.targetsize   	= sizeof(struct xt_net_detect_target_info),
	.table        	= "filter",
     	.me          	= THIS_MODULE,
};

/* Not __exit: called from init() */
static void fini(void)
{
	xt_unregister_target(&netdetect);
}

static int __init init(void)
{
	memset(ND_history, 0, sizeof(struct net_detect_history) * MAX_HISTORY);
	memset(NDip_history, 0, sizeof(struct net_detect_history) * 255);
	init_net_detect_proc();

	if (xt_register_target(&netdetect))
	{
		printk("NETDETECT target register ERROR !\n");
		return -EINVAL;
	}

	printk("NETDETECT target register\n");

	return 0;
}


module_init(init);
module_exit(fini);

EXPORT_SYMBOL(ND_history);
EXPORT_SYMBOL(NDip_history);
EXPORT_SYMBOL(ND_limit_burst);

