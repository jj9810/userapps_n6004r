/*
 * FILE NAME rtl_gpio.c
 *
 * BRIEF MODULE DESCRIPTION
 *  GPIO For Flash Reload Default
 *
 *  Author: jimmylin@realtek.com.tw
 *
 *  Copyright (c) 2011 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

//#define CONFIG_USING_JTAG 1

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/reboot.h>
#include <linux/kmod.h>
#include <linux/proc_fs.h>
#include  "bspchip.h"
#define AUTO_CONFIG

#if defined (CONFIG_RTL_8196D) || defined(CONFIG_RTL_8196E)
extern unsigned int get_8192cd_gpio0_7();
#endif

// 2009-0414
//#define	DET_WPS_SPEC
#ifndef CONFIG_RTK_VOIP_DRIVERS_ATA_DECT //DECT SPI use GPIO E interrupt, need refine code to share irq.
#ifndef CONFIG_SERIAL_SC16IS7X0 //SC16IS7x0 use GPIO E interrupt, too.  
//#define USE_INTERRUPT_GPIO	//undefine USE_INTERRUPT_GPIO
#endif
#endif

/*
enabled immediate mode pbc ; must enabled "USE_INTERRUPT_GPIO" and "IMMEDIATE_PBC"
gpio will rx pbc event and trigger wscd by signal method
note:also need enabled IMMEDIATE_PBC at wsc.h (sdk/users/wsc/src/)
*/
//#define USE_INTERRUPT_GPIO	//undefine USE_INTERRUPT_GPIO
//#define IMMEDIATE_PBC 


#ifdef IMMEDIATE_PBC
int	wscd_pid = 0;
struct pid *wscd_pid_Ptr=NULL;
#endif

#if defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)
#ifndef CONFIG_RTK_VOIP_BOARD
	#define READ_RF_SWITCH_GPIO
#endif
#endif

#if defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)
	#include "drivers/net/rtl819x/AsicDriver/rtl865xc_asicregs.h"
/*define the GPIO physical address to customer_gpio.h*/
#ifdef CONFIG_RTK_VOIP_BOARD
	#if defined (CONFIG_RTK_VOIP_GPIO_8954C_V100) || \
		defined (CONFIG_RTK_VOIP_GPIO_8964C_QA)
	

		// GPIO E7
		#define RESET_PIN_IOBASE PEFGH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_PIN_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_PIN_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_PIN_NO 7 					//pin number of the EFGH

		// GPIO G0
		#define RESET_LED_IOBASE PEFCH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_LED_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_LED_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_LED_NO 16 				//pin number of the EFGH
		
		// GPIO G1
		#define AUTOCFG_LED_IOBASE PEFGH_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_LED_DIRBASE PEFGH_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_LED_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_LED_NO 17 				//pin number of the EFGH

		// GPIO E2
		#define AUTOCFG_PIN_IOBASE PEFGH_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_PIN_DIRBASE PEFGH_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_PIN_DATABASE PEFGH_DAT  //RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_PIN_NO 2 				//pin number of the EFGH)

		#define AUTOCFG_PIN_IMR PEF_IMR
		#define RTL_GPIO_MUX_DATA 0x00004300 	//MUX for GPIO

	#elif defined(CONFIG_RTK_VOIP_GPIO_8954C_V200) || \
	 	  defined(CONFIG_RTK_VOIP_GPIO_8954C_V400)

		// GPIO F4 DEFAULT_Button
		#define RESET_PIN_IOBASE PEFGH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_PIN_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_PIN_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_PIN_NO 12 				//pin number of the EFGH

		// GPIO E7 SYS LED
		#define RESET_LED_IOBASE PEFGH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_LED_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_LED_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_LED_NO 7 					//number of the EFGH
		
		// GPIO F0 WPS LED
		#define AUTOCFG_LED_IOBASE PEFGH_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_LED_DIRBASE PEFGH_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_LED_DATABASE PEFGH_DAT  //RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_LED_NO 8 				//pin number of the EFGH

		// GPIO F3 WPS Button
		#define AUTOCFG_PIN_IOBASE PEFGH_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_PIN_DIRBASE PEFGH_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_PIN_DATABASE PEFGH_DAT  //RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_PIN_NO 11 				//pin number of the EFGH
		#define AUTOCFG_PIN_IMR PEF_IMR
		#define RTL_GPIO_MUX_DATA 0x00000300 	//MUX for GPIO

	#elif defined(CONFIG_RTK_VOIP_GPIO_8954C_SOUNDWIN_XVN1420)
		// GPIO F4 DEFAULT_Button
		#define RESET_PIN_IOBASE PEFGH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_PIN_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_PIN_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_PIN_NO 12 				//pin number of the EFGH
		
		// No SYS LED
		
		// No WPS LED
		
		// GPIO D1 WPS Button
		#define AUTOCFG_PIN_IOBASE PABCD_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_PIN_DIRBASE PABCD_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_PIN_DATABASE PABCD_DAT  //RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_PIN_NO 25 				//pin number of the EFGH
		#define AUTOCFG_PIN_IMR PCD_IMR
		#define RTL_GPIO_MUX_DATA 0x00000300 	//MUX for GPIO

	#elif defined(CONFIG_RTK_VOIP_GPIO_8954C_PMC)

                // GPIO A3 DEFAULT_Button
                #define RESET_PIN_IOBASE PABCD_CNR              //RTL_GPIO_PABCD_CNR
                #define RESET_PIN_DIRBASE PABCD_DIR             //RTL_GPIO_PABCD_DIR
                #define RESET_PIN_DATABASE PABCD_DAT    //RTL_GPIO_PABCD_DATA
                #define RESET_PIN_NO 3                                 //pin number of the ABCD

                // GPIO C1 SYS LED
                #define RESET_LED_IOBASE PABCD_CNR              //RTL_GPIO_PABCD_CNR
                #define RESET_LED_DIRBASE PABCD_DIR             //RTL_GPIO_PABCD_DIR
                #define RESET_LED_DATABASE PABCD_DAT    //RTL_GPIO_PABCD_DATA
                #define RESET_LED_NO 16                                  //number of the ABCD

                // GPIO A1 WPS LED
                #define AUTOCFG_LED_IOBASE PABCD_CNR    //RTL_GPIO_PABCD_CNR
                #define AUTOCFG_LED_DIRBASE PABCD_DIR   //RTL_GPIO_PABCD_DIR
                #define AUTOCFG_LED_DATABASE PABCD_DAT  //RTL_GPIO_PABCD_DATA
                #define AUTOCFG_LED_NO 1                                //pin number of the ABCD

                // GPIO A2 WPS Button
                #define AUTOCFG_PIN_IOBASE PABCD_CNR    //RTL_GPIO_PABCD_CNR
                #define AUTOCFG_PIN_DIRBASE PABCD_DIR   //RTL_GPIO_PABCD_DIR
                #define AUTOCFG_PIN_DATABASE PABCD_DAT  //RTL_GPIO_PABCD_DATA
                #define AUTOCFG_PIN_NO 2                               //pin number of the ABCD
                #define AUTOCFG_PIN_IMR PEF_IMR
                #define RTL_GPIO_MUX_DATA 0x00000018    //MUX for GPIO
		#define RTL_GPIO_MUX2_DATA 0x00038000
	#elif defined(CONFIG_RTK_VOIP_GPIO_8972D_V100)
		// GPIO G2 DEFAULT_Button
		#define RESET_PIN_IOBASE PEFGH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_PIN_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_PIN_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_PIN_NO 18 				//pin number of the EFGH

		// GPIO G3 SYS LED
		#define RESET_LED_IOBASE PEFGH_CNR 		//RTL_GPIO_PEFGH_CNR
		#define RESET_LED_DIRBASE PEFGH_DIR		//RTL_GPIO_PEFGH_DIR 
		#define RESET_LED_DATABASE PEFGH_DAT 	//RTL_GPIO_PEFGH_DATA
		#define RESET_LED_NO 19 					//number of the EFGH
		
		// GPIO G4 WPS LED
		#define AUTOCFG_LED_IOBASE PEFGH_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_LED_DIRBASE PEFGH_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_LED_DATABASE PEFGH_DAT  //RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_LED_NO 20 				//pin number of the EFGH

		// GPIO G1 WPS Button
		#define AUTOCFG_PIN_IOBASE PEFGH_CNR 	//RTL_GPIO_PEFGH_CNR
		#define AUTOCFG_PIN_DIRBASE PEFGH_DIR	//RTL_GPIO_PEFGH_DIR 
		#define AUTOCFG_PIN_DATABASE PEFGH_DAT  //RTL_GPIO_PEFGH_DATA
		#define AUTOCFG_PIN_NO 17 				//pin number of the EFGH
		#define AUTOCFG_PIN_IMR PEF_IMR
  
    		#define RTL_GPIO_MUX_DATA 0x00000C00 	//MUX for GPIO
    #endif
	
	#define RTL_GPIO_MUX 0xB8000040

	//#define RTL_GPIO_WIFI_ONOFF     19
	#define AUTOCFG_BTN_PIN         AUTOCFG_PIN_NO
	#define AUTOCFG_LED_PIN         AUTOCFG_LED_NO
	#define RESET_LED_PIN           RESET_LED_NO
	#define RESET_BTN_PIN           RESET_PIN_NO
	
#else
#if defined(CONFIG_RTL_8196C)	
   #if defined(CONFIG_RTL_8196CS)
	#define PCIE0_BASE 0xb9000000
	#define PCIE_BASE_OFFSET (PCIE0_BASE+0x40)
	#define PCIE_PIN_MUX PCIE_BASE_OFFSET
	#define RESET_PIN_IOBASE (PCIE_BASE_OFFSET+4)
	#define WPS_PIN_IOBASE (PCIE_BASE_OFFSET+4)
	#define WPS_LED_IOBASE (PCIE_BASE_OFFSET)
	#define PCIE_GPIO_IMR (PCIE_BASE_OFFSET+8)
	#define MODE_MARSK 24
	#define MODE_MARSK1 28
	#define DIR_MASK 16
	#define DIR_MASK1 24
	#define IN_MASK 0
	#define OUT_MASK 8
	#define OUT_MASK1 20
	
	// GPIO A0
	//#define RESET_PIN_IOBASE RESET_PIN_IOBASE 	//RTL_GPIO_PABCD_CNR
	#define RESET_PIN_DIRBASE RESET_PIN_IOBASE	//RTL_GPIO_PABCD_DIR 
	#define RESET_PIN_DATABASE RESET_PIN_IOBASE //RTL_GPIO_PABCD_DATA
	#define RESET_PIN_NO 0 /*number of the ABCD*/

	// GPIO C2
	#define RESET_LED_IOBASE PABCD_CNR 	//RTL_GPIO_PABCD_CNR
	#define RESET_LED_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define RESET_LED_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_LED_NO 2 /*number of the ABCD*/

	// GPIO C4
	#define AUTOCFG_LED_IOBASE WPS_LED_IOBASE 	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_LED_DIRBASE WPS_LED_IOBASE	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_LED_DATABASE WPS_LED_IOBASE //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_LED_NO 20 /*number of the ABCD*/

	// GPIO A1
	#define AUTOCFG_PIN_IOBASE WPS_PIN_IOBASE 	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_PIN_DIRBASE WPS_PIN_IOBASE	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_PIN_DATABASE WPS_PIN_IOBASE //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_PIN_NO 2 /*number of the ABCD)*/
	#define AUTOCFG_PIN_IMR PCIE_GPIO_IMR
	#else
	// GPIO A0
	#define RESET_PIN_IOBASE PABCD_CNR 	//RTL_GPIO_PABCD_CNR
	#define RESET_PIN_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define RESET_PIN_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_PIN_NO 0 /*number of the ABCD*/

	// GPIO C2
	#define RESET_LED_IOBASE PABCD_CNR 	//RTL_GPIO_PABCD_CNR
	#define RESET_LED_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define RESET_LED_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_LED_NO 18 /*number of the ABCD*/

	// GPIO C4
	#define AUTOCFG_LED_IOBASE PABCD_CNR 	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_LED_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_LED_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_LED_NO 20 /*number of the ABCD*/

	// GPIO A1
	#define AUTOCFG_PIN_IOBASE PABCD_CNR 	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_PIN_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_PIN_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_PIN_NO 1 /*number of the ABCD)*/
	#define AUTOCFG_PIN_IMR PAB_IMR
	#endif
#ifdef CONFIG_POCKET_ROUTER_SUPPORT
	#define RTL_GPIO_MUX_GPIOA2	(3<<20)
	#define RTL_GPIO_MUX_GPIOB3	(3<<2)
	#define RTL_GPIO_MUX_GPIOB2	(3<<0)	
	#define RTL_GPIO_MUX_GPIOC0	(3<<12)
	#define RTL_GPIO_MUX_POCKETAP_DATA	(RTL_GPIO_MUX_GPIOA2 | RTL_GPIO_MUX_GPIOB3 | RTL_GPIO_MUX_GPIOB2 | RTL_GPIO_MUX_GPIOC0)

	#define RTL_GPIO_CNR_GPIOA2	(1<<2)
	#define RTL_GPIO_CNR_GPIOB3	(1<<11)
	#define RTL_GPIO_CNR_GPIOB2	(1<<10)	
	#define RTL_GPIO_CNR_GPIOC0	(1<<16)
	#define RTL_GPIO_CNR_POCKETAP_DATA	(RTL_GPIO_CNR_GPIOA2 | RTL_GPIO_CNR_GPIOB3 | RTL_GPIO_CNR_GPIOB2 | RTL_GPIO_CNR_GPIOC0)

	#define RTL_GPIO_DIR_GPIOA2	(1<<2) /* &- */
	#define RTL_GPIO_DIR_GPIOB3	(1<<11) /* &- */
	#define RTL_GPIO_DIR_GPIOB2	(1<<10) /* |*/	
	#define RTL_GPIO_DIR_GPIOC0	(1<<16) /* &- */

	#define RTL_GPIO_DAT_GPIOA2	(1<<2) 
	#define RTL_GPIO_DAT_GPIOB3	(1<<11) 
	#define RTL_GPIO_DAT_GPIOB2	(1<<10) 	
	#define RTL_GPIO_DAT_GPIOC0	(1<<16) 

	static int ap_cli_rou_time_state[2] = {0};
	static char ap_cli_rou_state = 0;
	static char ap_cli_rou_idx=0;
	static char pocketAP_hw_set_flag='0';

	static char dc_pwr_plugged_time_state = 0;
	static char dc_pwr_plugged_state = 0;
	static char dc_pwr_plugged_flag = '0';

	static int pwr_saving_state=0;
	static char pwr_saving_led_toggle = 0;
#endif

	
#elif defined(CONFIG_RTL_8198)// || defined(CONFIG_RTL_819XD) //LZQ
	// GPIO H1
	#define RESET_PIN_IOBASE PEFGH_CNR 	//RTL_GPIO_PABCD_CNR
	#define RESET_PIN_DIRBASE PEFGH_DIR	//RTL_GPIO_PABCD_DIR 
	#define RESET_PIN_DATABASE PEFGH_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_PIN_NO 25 /*number of the ABCD*/

	// GPIO H3
	#define RESET_LED_IOBASE PEFGH_CNR 	//RTL_GPIO_PABCD_CNR
	#define RESET_LED_DIRBASE PEFGH_DIR	//RTL_GPIO_PABCD_DIR 
	#define RESET_LED_DATABASE PEFGH_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_LED_NO 27 /*number of the ABCD*/

	// GPIO G4
	#define AUTOCFG_LED_IOBASE PEFGH_CNR 	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_LED_DIRBASE PEFGH_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_LED_DATABASE PEFGH_DAT //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_LED_NO 20 /*number of the ABCD*/

	// GPIO E1
	#define AUTOCFG_PIN_IOBASE PEFGH_CNR 	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_PIN_DIRBASE PEFGH_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_PIN_DATABASE PEFGH_DAT //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_PIN_NO 1 /*number of the ABCD)*/
	#define AUTOCFG_PIN_IMR PGH_IMR

	
#elif defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)	//LZQ
	#if defined(CONFIG_RTL_8881A)
	//V720B
	#define RESET_PIN_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
	#define RESET_PIN_DIRBASE 	PABCD_DIR //RTL_GPIO_PABCD_DIR 
	#define RESET_PIN_DATABASE 	PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_PIN_NO 4 /*number of the ABCD*/

	#define RESET_LED_IOBASE 	PEFGH_CNR	//RTL_GPIO_PABCD_CNR
	#define RESET_LED_DIRBASE 	PEFGH_DIR //RTL_GPIO_PABCD_DIR 
	#define RESET_LED_DATABASE 	PEFGH_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_LED_NO 3 /*number of the ABCD*/

	#define AUTOCFG_LED_IOBASE 	PEFGH_CNR	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_LED_DIRBASE PEFGH_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_LED_DATABASE PEFGH_DAT //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_LED_NO 3 /*number of the ABCD*/

	#define AUTOCFG_PIN_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_PIN_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_PIN_DATABASE PABCD_DAT 	//RTL_GPIO_PABCD_DATA
	#define AUTOCFG_PIN_NO 3					/*number of the ABCD)*/
	#define AUTOCFG_PIN_IMR 	PAB_IMR
	
	
	#else
	//V201 demo board 
	#define RESET_PIN_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
	#define RESET_PIN_DIRBASE 	PABCD_DIR //RTL_GPIO_PABCD_DIR 
	#define RESET_PIN_DATABASE 	PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_PIN_NO 25 /*number of the ABCD*/

	#define RESET_LED_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
	#define RESET_LED_DIRBASE 	PABCD_DIR //RTL_GPIO_PABCD_DIR 
	#define RESET_LED_DATABASE 	PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define RESET_LED_NO 27 /*number of the ABCD*/

	#define AUTOCFG_LED_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_LED_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_LED_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define AUTOCFG_LED_NO 20 /*number of the ABCD*/

	#define AUTOCFG_PIN_IOBASE 	PABCD_CNR	//RTL_GPIO_PABCD_CNR
	#define AUTOCFG_PIN_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define AUTOCFG_PIN_DATABASE PABCD_DAT 	//RTL_GPIO_PABCD_DATA
	#define AUTOCFG_PIN_NO 1 					/*number of the ABCD)*/
	#define AUTOCFG_PIN_IMR 	PAB_IMR
	#endif
	#if defined (CONFIG_RTL_8197D)
		#define USB_MODE_DETECT_PIN_NO			1
		#define USB_MODE_DETECT_PIN_IOBASE		PABCD_DAT
	#elif defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8196E)
		#define USB_MODE_DETECT_PIN_NO			4
		//#define USB_MODE_DETECT_PIN_IOBASE		PABCD_DAT
	#endif
#endif
	// GPIO C3
	#define WIFI_ONOFF_PIN_IOBASE PABCD_CNR 	//RTL_GPIO_PABCD_CNR
	#define WIFI_ONOFF_PIN_DIRBASE PABCD_DIR	//RTL_GPIO_PABCD_DIR 
	#define WIFI_ONOFF_PIN_DATABASE PABCD_DAT //RTL_GPIO_PABCD_DATA
	#define WIFI_ONOFF_PIN_NO 19/*umber of the ABCD)*/
	#define WIFI_ONOFF_PIN_IMR PAB_IMR

/*
	#define RTL_GPIO_MUX 0xB8000030
	#define RTL_GPIO_MUX_DATA 0x0FC00380//for WIFI ON/OFF and GPIO

	

	#define AUTOCFG_BTN_PIN	AUTOCFG_PIN_NO	// 1
	#define AUTOCFG_LED_PIN		AUTOCFG_LED_NO//20
	#define RESET_LED_PIN		RESET_LED_NO //18
	#define RESET_BTN_PIN		RESET_PIN_NO //0
	#define RTL_GPIO_WIFI_ONOFF	WIFI_ONOFF_PIN_NO
*/
	 #define RTL_GPIO_MUX 0xB8000040
	#ifdef CONFIG_8198_PORT5_GMII
		#define RTL_GPIO_MUX_DATA 0x00340000
	#else 
		#define RTL_GPIO_MUX_DATA 0x00340C00//for WIFI ON/OFF and GPIO
	#endif 
	#define RTL_GPIO_WIFI_ONOFF     19

#if defined(CONFIG_RTL_8196C)	 
	#if  defined(CONFIG_RTL_8196CS)
	 #define AUTOCFG_BTN_PIN         2
	 #define AUTOCFG_LED_PIN         1
	 #define RESET_LED_PIN           2
	 #define RESET_BTN_PIN           0
	#else
	 #define AUTOCFG_BTN_PIN         3
	 #define AUTOCFG_LED_PIN         4
	 #define RESET_LED_PIN           6
	 #define RESET_BTN_PIN           5
	#endif
	
#elif defined(CONFIG_RTL_8198) //|| defined(CONFIG_RTL_819XD) LZQ
	 #define AUTOCFG_BTN_PIN         24
	 #define AUTOCFG_LED_PIN         26
	 #define RESET_LED_PIN           27
	 #define RESET_BTN_PIN           25

#elif defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)
    #if defined(CONFIG_RTL_8881A)
    #define RTL_GPIO_MUX_GPIOE2 (3<<7)
	#endif
	#define RTL_GPIO_MUX_GPIOA2_6 (6<<0)
	#define RTL_GPIO_MUX_GPIOA0_1 (3<<12)
	#define RTL_GPIO_MUX_2_GPIOB7 (4<<15)	
	#define RTL_GPIO_MUX_2_GPIOC0 (4<<18)
	
	#define RTL_GPIO_CNR_GPIOA1 (1<<1)
	#define RTL_GPIO_CNR_GPIOA2 (1<<2)
	#define RTL_GPIO_CNR_GPIOA3 (1<<3)
	#define RTL_GPIO_CNR_GPIOA4 (1<<4)
	#define RTL_GPIO_CNR_GPIOA5 (1<<5)
	#define RTL_GPIO_CNR_GPIOA6 (1<<6)
	#define RTL_GPIO_CNR_GPIOB7 (1<<15) 
	#define RTL_GPIO_CNR_GPIOC0 (1<<16)

	#define RTL_GPIO_DIR_GPIOA1 (1<<1) /* &- */
	#define RTL_GPIO_DIR_GPIOA2 (1<<2) /* |*/
	#define RTL_GPIO_DIR_GPIOA3 (1<<3) /* &-*/	
	#define RTL_GPIO_DIR_GPIOA4 (1<<4) /* &- */
	#define RTL_GPIO_DIR_GPIOA5 (1<<5) /* &- */
	#define RTL_GPIO_DIR_GPIOA6 (1<<6) /* | */
	#define RTL_GPIO_DIR_GPIOB7 (1<<15) /* &-*/	
	#define RTL_GPIO_DIR_GPIOC0 (1<<16) /* &- */

	#define RTL_GPIO_DAT_GPIOA1 (1<<1) 
	#define RTL_GPIO_DAT_GPIOA2 (1<<2) 
	#define RTL_GPIO_DAT_GPIOA3 (1<<3) 	
	#define RTL_GPIO_DAT_GPIOA4 (1<<4) 
	#define RTL_GPIO_DAT_GPIOA5 (1<<5) 
	#define RTL_GPIO_DAT_GPIOA6 (1<<6) 
	#define RTL_GPIO_DAT_GPIOB7 (1<<15) 	
	#define RTL_GPIO_DAT_GPIOC0 (1<<16) 

	#if defined(CONFIG_RTL_8197D)
		#define RTL_GPIO_MUX_POCKETAP_DATA		(RTL_GPIO_MUX_GPIOA0_1 | RTL_GPIO_MUX_GPIOA2_6)
		#define RTL_GPIO_MUX_2_POCKETAP_DATA	(RTL_GPIO_MUX_2_GPIOB7 | RTL_GPIO_MUX_2_GPIOC0)
		#define RTL_GPIO_CNR_POCKETAP_DATA		(RTL_GPIO_CNR_GPIOA1 | \
												 RTL_GPIO_CNR_GPIOA2 | \
												 RTL_GPIO_CNR_GPIOA3 | \
												 RTL_GPIO_CNR_GPIOA4 | \
												 RTL_GPIO_CNR_GPIOA5 | \
												 RTL_GPIO_CNR_GPIOA6 | \
												 RTL_GPIO_CNR_GPIOB7 | \
												 RTL_GPIO_CNR_GPIOC0)

	
		#define AUTOCFG_LED_PIN 			6
		#define AUTOCFG_BTN_PIN         	3
		#define RESET_LED_PIN           	6//reset led will be turn off by timer function
		#define RESET_BTN_PIN           	5
	#elif defined(CONFIG_RTL_8197DL)
		#define RTL_GPIO_MUX_POCKETAP_DATA		(RTL_GPIO_MUX_GPIOA2_6)
		#define RTL_GPIO_CNR_POCKETAP_DATA		(RTL_GPIO_CNR_GPIOA2 | \
								 RTL_GPIO_CNR_GPIOA3 | \
								 RTL_GPIO_CNR_GPIOA4 | \
								 RTL_GPIO_CNR_GPIOA5 | \
								 RTL_GPIO_CNR_GPIOA6)

	
		#define AUTOCFG_LED_PIN 		6
		#define AUTOCFG_BTN_PIN         	3
		#define RESET_LED_PIN           	6//reset led will be turn off by timer function
		#define RESET_BTN_PIN           	5
	#elif defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8196E)
		#if defined(CONFIG_RTL_8881A)
		#define RTL_GPIO_MUX_POCKETAP_DATA	(RTL_GPIO_MUX_GPIOE2)
		#define RTL_GPIO_CNR_POCKETAP_DATA	(RTL_GPIO_CNR_GPIOA2 | \
											 RTL_GPIO_CNR_GPIOA4 | \
											 RTL_GPIO_CNR_GPIOA5 | \
											 RTL_GPIO_CNR_GPIOA6)

		#define AUTOCFG_LED_PIN 			2
		#define AUTOCFG_BTN_PIN         	3
		#define RESET_LED_PIN           	2
		#define RESET_BTN_PIN           	4
		#else
		#define RTL_GPIO_MUX_POCKETAP_DATA	(RTL_GPIO_MUX_GPIOA2_6)
		#define RTL_GPIO_CNR_POCKETAP_DATA	(RTL_GPIO_CNR_GPIOA2 | \
											 RTL_GPIO_CNR_GPIOA4 | \
											 RTL_GPIO_CNR_GPIOA5 | \
											 RTL_GPIO_CNR_GPIOA6)

	
		#define AUTOCFG_LED_PIN 			6
		#define AUTOCFG_BTN_PIN         	2
		#define RESET_LED_PIN           	6
#if defined(CONFIG_RTL_96E_GPIOB5_RESET)
                #define RESET_BTN_PIN                   13
#else
		#define RESET_BTN_PIN           	5		
#endif
		#endif
	
		
	
	#endif
#endif	 

#endif // CONFIG_RTK_VOIP
#endif // CONFIG_RTL_8196C || CONFIG_RTL_8198

// 2009-0414
#ifdef USE_INTERRUPT_GPIO
#ifdef CONFIG_RTK_VOIP
	#define GPIO_IRQ_NUM		(16+17)	// GPIO_EFGH
#else
	#define GPIO_IRQ_NUM		1	
#endif
#endif

	#define PROBE_TIME	5


#define PROBE_NULL		0
#define PROBE_ACTIVE	1
#define PROBE_RESET		2
#define PROBE_RELOAD	3
#define RTL_R32(addr)		(*(volatile unsigned long *)(addr))
#define RTL_W32(addr, l)	((*(volatile unsigned long*)(addr)) = (l))
#define RTL_R8(addr)		(*(volatile unsigned char*)(addr))
#define RTL_W8(addr, l)		((*(volatile unsigned char*)(addr)) = (l))

//#define  GPIO_DEBUG
#ifdef GPIO_DEBUG
/* note: prints function name for you */
#  define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)
#else
#  define DPRINTK(fmt, args...)
#endif

static struct timer_list probe_timer;
#ifdef CONFIG_EFM_PATCH
static struct timer_list wps_timer;
#endif
static unsigned int    probe_counter;
static unsigned int    probe_state;

static char default_flag='0';
//Brad add for update flash check 20080711
int start_count_time=0;
int Reboot_Wait=0;

static int get_dc_pwr_plugged_state();

#if defined(USE_INTERRUPT_GPIO)
struct gpio_wps_device
{
	unsigned int name;
};
struct gpio_wps_device priv_gpio_wps_device;
#endif

//#ifdef CONFIG_RTL865X_AC

#ifdef CONFIG_POCKET_ROUTER_SUPPORT
static struct timer_list pocket_ap_timer;
#endif

#ifdef CONFIG_RTL_ULINKER
static struct timer_list ulinker_timer;
static struct timer_list ulinker_ap_cl_timer;
#endif

//#ifdef	USE_INTERRUPT_GPIO
static int wps_button_push = 0;
//#endif

#if defined(CONFIG_RTL_8196CS)
void update_pcie_status(void)
{
	unsigned int temp;
	temp=RTL_R32(0xb8b00728);
	temp=RTL_R32(PCIE_PIN_MUX);
	temp=RTL_R32(RESET_PIN_DATABASE);
	
	
	//printk("LINE: %x d:%x *  %x****R:%x\n",__LINE__,RTL_R32(0xb8b00728),RTL_R32(PCIE_PIN_MUX),RTL_R32(RESET_PIN_DATABASE));
}
#endif

#ifdef AUTO_CONFIG
static unsigned int		AutoCfg_LED_Blink;
static unsigned int		AutoCfg_LED_Toggle;
static unsigned int		AutoCfg_LED_Slow_Blink;
static unsigned int		AutoCfg_LED_Slow_Toggle;

void autoconfig_gpio_init(void)
{
#ifdef CONFIG_RTL_8198B
	//gpio58, WPS button
	REG32(0xBB00011C) |= (1<<25); //EN_GPIO
	REG32(0xBB0001D8) &= ~(1<<25); //SEL_GPIO select input mode

	//gpio61, use wlan1 led as WPS led, for test
	//REG32(0xBB00011C) |= (1<<28); //EN_GPIO
	//REG32(0xBB0001D8) |= (1<<28); //SEL_GPIO select output mode

	// turn off WPS led in the beginning
	// Strange!! Off the led here cause "LLT Polling failed". So remove it.
	//REG32(0xBB000108) = (1<<0); //CTRL_GPIO output 1, off
	//REG32(0xBB000104) |= (1<<28); //CTRL_GPIO output 1, off, for test
#elif defined(CONFIG_RTL_8196CS)
	RTL_W32(AUTOCFG_PIN_IOBASE,(RTL_R32(AUTOCFG_PIN_IOBASE)&(~((1 << AUTOCFG_BTN_PIN)<<MODE_MARSK))));
	RTL_W32(AUTOCFG_LED_IOBASE,(RTL_R32(AUTOCFG_LED_IOBASE)&(~((1 << AUTOCFG_LED_PIN)<<MODE_MARSK1))));

	// Set GPIOA pin 1 as input pin for auto config button
	RTL_W32(AUTOCFG_PIN_DIRBASE, (RTL_R32(AUTOCFG_PIN_DIRBASE) & (~(((1 << AUTOCFG_BTN_PIN))<<DIR_MASK1))));

	// Set GPIOA ping 3 as output pin for auto config led
	RTL_W32(AUTOCFG_LED_DIRBASE, (RTL_R32(AUTOCFG_LED_DIRBASE) | ((1 << AUTOCFG_LED_PIN)<<DIR_MASK1)));

	// turn off auto config led in the beginning
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | ((1 << AUTOCFG_LED_PIN)<<OUT_MASK1)));
	update_pcie_status();
	//printk("LINE: %x d:%x *  %x****R:%x\n",__LINE__,RTL_R32(0xb8b00728),RTL_R32(PCIE_PIN_MUX),RTL_R32(RESET_PIN_DATABASE));
#else
	RTL_W32(AUTOCFG_PIN_IOBASE,(RTL_R32(AUTOCFG_PIN_IOBASE)&(~(1 << AUTOCFG_BTN_PIN))));
#ifdef AUTOCFG_LED_NO
	RTL_W32(AUTOCFG_LED_IOBASE,(RTL_R32(AUTOCFG_LED_IOBASE)&(~(1 << AUTOCFG_LED_PIN))));
   #endif

	// Set GPIOA pin 1 as input pin for auto config button
	RTL_W32(AUTOCFG_PIN_DIRBASE, (RTL_R32(AUTOCFG_PIN_DIRBASE) & (~(1 << AUTOCFG_BTN_PIN))));

#ifdef AUTOCFG_LED_NO
	// Set GPIOA ping 3 as output pin for auto config led
	RTL_W32(AUTOCFG_LED_DIRBASE, (RTL_R32(AUTOCFG_LED_DIRBASE) | (1 << AUTOCFG_LED_PIN)));

	// turn off auto config led in the beginning
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
#endif
#endif
}
#ifdef CONFIG_RTL_8196C_GW_MP

void all_led_on(void)
{
	//printk("Into MP GPIO");
	 #ifdef CONFIG_RTL_8196C_GW_MP
	RTL_W32(0xB8000030, (RTL_R32(0xB8000030) | 0x00F00F80 ));
	RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(1 << PS_LED_GREEN_PIN))));
	RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(1 << PS_LED_ORANGE_PIN))));

	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (1 << PS_LED_GREEN_PIN)));
	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (1 << PS_LED_ORANGE_PIN)));
	
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) | (1 << PS_LED_GREEN_PIN)));
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << PS_LED_ORANGE_PIN))));

	/* inet_led init setting */
	RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(1 << INET_LED_GREEN_PIN))));
	RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(1 << INET_LED_ORANGE_PIN))));

	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (1 << INET_LED_GREEN_PIN)));
	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (1 << INET_LED_ORANGE_PIN)));
	
	RTL_W32(AUTOCFG_LED_DIRBASE, (RTL_R32(AUTOCFG_LED_DIRBASE) & (~(1 << AUTOCFG_LED_PIN_MP))));

	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (1 << AUTOCFG_LED_PIN_MP)));

	//RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << INET_LED_GREEN_PIN))));
	//RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) | (1 << INET_LED_ORANGE_PIN)));
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << PS_LED_GREEN_PIN))));	
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << PS_LED_ORANGE_PIN))));
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << INET_LED_GREEN_PIN))));
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << INET_LED_ORANGE_PIN))));
	RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(1 << AUTOCFG_LED_PIN_MP))));
	RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN_MP))));
	#endif

}
#endif

#ifdef CONFIG_RTL_8198B
void autoconfig_gpio_off(void)
{
	//RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
	REG32(0xBB000108) |= (1<<0); //CTRL_GPIO output 1, off
	//REG32(0xBB000104) |= (1<<28); //CTRL_GPIO output 1, off, for test
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_on(void)
{
	//RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
	REG32(0xBB000108) &= ~(1<<0); //CTRL_GPIO output 0, on
	//REG32(0xBB000104) &= ~(1<<28); //CTRL_GPIO output 0, on, for test
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_blink(void)
{
	//RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
	REG32(0xBB000108) &= ~(1<<0); //CTRL_GPIO output 0, on
	//REG32(0xBB000104) &= ~(1<<28); //CTRL_GPIO output 0, on, for test
	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
	AutoCfg_LED_Slow_Blink = 0;
}

void autoconfig_gpio_slow_blink(void)
{
	//RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
	REG32(0xBB000108) &= ~(1<<0); //CTRL_GPIO output 0, on
	//REG32(0xBB000104) &= ~(1<<28); //CTRL_GPIO output 0, on, for test
	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
	AutoCfg_LED_Slow_Blink = 1;
	AutoCfg_LED_Slow_Toggle = 1;
}
#elif defined(CONFIG_RTL_8196CS)
void autoconfig_gpio_off(void)
{
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | ((1 << AUTOCFG_LED_PIN)<<OUT_MASK1)));
	AutoCfg_LED_Blink = 0;
	update_pcie_status();
	//printk("LINE: %x d:%x *  %x****R:%x\n",__LINE__,RTL_R32(0xb8b00728),RTL_R32(PCIE_PIN_MUX),RTL_R32(RESET_PIN_DATABASE));
}

void autoconfig_gpio_on(void)
{
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~((1 << AUTOCFG_LED_PIN)<<OUT_MASK1))));
	//printk("LINE: %x d:%x *  %x****R:%x\n",__LINE__,RTL_R32(0xb8b00728),RTL_R32(PCIE_PIN_MUX),RTL_R32(RESET_PIN_DATABASE));
	update_pcie_status();
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_blink(void)
{
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~((1 << AUTOCFG_LED_PIN)<<OUT_MASK1))));
	//printk("LINE: %x d:%x *  %x****R:%x\n",__LINE__,RTL_R32(0xb8b00728),RTL_R32(PCIE_PIN_MUX),RTL_R32(RESET_PIN_DATABASE));
	update_pcie_status();
	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;

}

#elif defined(CONFIG_RTL_ULINKER)
extern void RTLWIFINIC_GPIO_write(unsigned int gpio_num, unsigned int value);
void autoconfig_gpio_off(void)
{
	RTLWIFINIC_GPIO_write(4, 0);
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_on(void)
{
	RTLWIFINIC_GPIO_write(4, 1);
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_blink(void)
{
	RTLWIFINIC_GPIO_write(4, 1);

	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
	AutoCfg_LED_Slow_Blink = 0;

}

void autoconfig_gpio_slow_blink(void)
{
	RTLWIFINIC_GPIO_write(4, 1);

	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
	AutoCfg_LED_Slow_Blink = 1;
	AutoCfg_LED_Slow_Toggle = 1;

}
#else
void autoconfig_gpio_off(void)
{
   #ifdef AUTOCFG_LED_NO
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
   #endif
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_on(void)
{
   #ifdef AUTOCFG_LED_NO
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
   #endif
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_blink(void)
{
   #ifdef AUTOCFG_LED_NO
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
   #endif
	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
	AutoCfg_LED_Slow_Blink = 0;

}

void autoconfig_gpio_slow_blink(void)
{
   #ifdef AUTOCFG_LED_NO
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
   #endif	
	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
	AutoCfg_LED_Slow_Blink = 1;
	AutoCfg_LED_Slow_Toggle = 1;

}
#endif
#endif // AUTO_CONFIG


#ifdef CONFIG_EFM_PATCH
#undef USE_IRQ_FOR_WPS

#if defined CONFIG_RTL_8197DL || defined CONFIG_RTL_8197D || defined CONFIG_RTL_8196D || defined CONFIG_RTL_8196E || defined CONFIG_RTL_8196C

#define RUN_LED_GPIO 6

#define USB_LED_GPIO (8+8)  // C0  */

#define RUN_LED_BIT (0x1<<RUN_LED_GPIO) /* PAC */

#define USB_LED_BIT (0x1<<USB_LED_GPIO) /* PAC */


#define RESET_BT_GPIO 5
#define RESET_BT_GPIO_BIT (0x1<<RESET_BT_GPIO)

#if defined CONFIG_RTL_8812_SUPPORT
/* For RTL8197D + 11AC board */
#define WPS_BT_GPIO 3
#else
/* For 11N board */
#define WPS_BT_GPIO 2
#endif

#define WPS_BT_GPIO_BIT (0x1<<WPS_BT_GPIO)


#else

#define RESET_BT_GPIO 25
#define RESET_BT_GPIO_BIT (0x1<<RESET_BT_GPIO)
#define RUN_LED_GPIO 27
#define RUN_LED_BIT (0x1<<RUN_LED_GPIO) /* PAC */

#define WPS_BT_GPIO 24
#define WPS_BT_GPIO_BIT (0x1<<WPS_BT_GPIO)

#endif


#ifdef CONFIG_RTL_8881A

#undef RESET_BT_GPIO
#undef RESET_BT_GPIO_BIT
#undef RUN_LED_GPIO
#undef RUN_LED_BIT

#define RESET_BT_GPIO 4  /* A4 */
#define RESET_BT_GPIO_BIT (0x1<<RESET_BT_GPIO)
#define RUN_LED_GPIO 2   /* E2 */
#define RUN_LED_BIT (0x1<<RUN_LED_GPIO) /* PAC */

#endif

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
#define EXT_INDICATION_GPIO 	2  /* A2 */
#define EXT_INDICATION_GPIO_BIT  (0x1<<EXT_INDICATION_GPIO)  /* A2 */
#endif


int run_period = HZ;
#define RUN_LED_BLINK 0x1
#define RUN_LED_ON 0x2
#define RUN_LED_OFF 0x3
#define RUN_LED_SLOW_BLINK 0x4
#define RUN_LED_FAST_BLINK 0x5

int run_blink = RUN_LED_BLINK;
int reset_op_start;


#if defined CONFIG_RTL_8197DL || defined CONFIG_RTL_8197D || defined CONFIG_RTL_8196D

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
static int check_ext2g_wps_bt(void)
{
	return 0;
}

static int check_ext5g_wps_bt(void)
{
	return 0;
}
#endif

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
#define LED_24G_ON_FLAG 0x800000
#define LED_5G_ON_FLAG  0x8000
extern unsigned long wlan0_ioaddr;

int wlan0_led_shadow;
int ext2g_led_on(void)
{
	if(!wlan0_ioaddr) return 0;
	wlan0_led_shadow |= LED_24G_ON_FLAG;
        RTL_W32(wlan0_ioaddr+0x4c, wlan0_led_shadow);
        return 0;
}

int ext2g_led_off(void)
{
	if(!wlan0_ioaddr) return 0;
	wlan0_led_shadow &= ~LED_24G_ON_FLAG;
        RTL_W32(wlan0_ioaddr+0x4c, wlan0_led_shadow);
        return 0;
}

int ext5g_led_on(void)
{
	if(!wlan0_ioaddr) return 0;
	wlan0_led_shadow |= LED_5G_ON_FLAG;
        RTL_W32(wlan0_ioaddr+0x4c, wlan0_led_shadow);
        return 0;
}

int ext5g_led_off(void)
{
	if(!wlan0_ioaddr) return 0;

	wlan0_led_shadow &= ~LED_5G_ON_FLAG;
        RTL_W32(wlan0_ioaddr+0x4c, wlan0_led_shadow);
        return 0;
}

int blink_ext_led(int band)
{
	if(!wlan0_ioaddr) return 0;

        if(wlan0_led_shadow & ((band==2)?LED_24G_ON_FLAG:LED_5G_ON_FLAG))
                wlan0_led_shadow &= ~((band==2)?LED_24G_ON_FLAG:LED_5G_ON_FLAG);
        else
                wlan0_led_shadow |= ((band==2)?LED_24G_ON_FLAG:LED_5G_ON_FLAG);
        RTL_W32(wlan0_ioaddr+0x4c, wlan0_led_shadow);
        return 0;
}

int gpio_shadow_abcd;
int ext_indication_led_on(void)
{
        gpio_shadow_abcd = RTL_R32(PABCD_DAT);
        gpio_shadow_abcd &= ~EXT_INDICATION_GPIO_BIT;
        RTL_W32(PABCD_DAT, gpio_shadow_abcd);
        return 0;
}

int ext_indication_led_off(void)
{
        gpio_shadow_abcd = RTL_R32(PABCD_DAT);
        gpio_shadow_abcd |= EXT_INDICATION_GPIO_BIT;
        RTL_W32(PABCD_DAT, gpio_shadow_abcd);
        return 0;
}


int ext_2g_led_op = -1;
int ext_5g_led_op = -1;

int ext_led_operation(int band, int op, int count)
{
        if(op == RUN_LED_ON)
        {
		if(band == 2) ext2g_led_on();
		else  ext5g_led_on();
                return 0;
        }

        if(op == RUN_LED_OFF)
        {
		if(band == 2) ext2g_led_off();
		else  ext5g_led_off();
                return 0;
        }

        if((op == RUN_LED_SLOW_BLINK) && (!(count%5)))
        {
                blink_ext_led(band);
        }
        else if(op == RUN_LED_FAST_BLINK)
                blink_ext_led(band);
}
#endif

#ifdef CONFIG_RTL_8881A
static int check_reset_bt(void)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PABCD_DAT);
        if(gpio_value & RESET_BT_GPIO_BIT)
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}


#ifdef WPS_BT_GPIO_BIT
static int check_wps_bt(void)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PABCD_DAT);
        if(gpio_value & WPS_BT_GPIO_BIT)
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}
#endif




int gpio_shadow;
int DIL_run_led_toggle(void)
{
        gpio_shadow = RTL_R32(PEFGH_DAT);
//      printk("-> GPIO_DATA: %08x\n", gpio_shadow);
        if( gpio_shadow & RUN_LED_BIT)
                gpio_shadow &= ~RUN_LED_BIT;
        else
                gpio_shadow |= RUN_LED_BIT;
        RTL_W32(PEFGH_DAT, gpio_shadow);
        return 0;
}


int DIL_run_led_on(void)
{
        gpio_shadow = RTL_R32(PEFGH_DAT);
        gpio_shadow &= ~RUN_LED_BIT;
        RTL_W32(PEFGH_DAT, gpio_shadow);
        return 0;
}

int DIL_run_led_off(void)
{
        gpio_shadow = RTL_R32(PEFGH_DAT);
        gpio_shadow |= RUN_LED_BIT;
        RTL_W32(PEFGH_DAT, gpio_shadow);
        return 0;
}
#else


static int check_reset_bt(void)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PABCD_DAT);
        if(gpio_value & RESET_BT_GPIO_BIT)
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
int check_bt(int gpio)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PABCD_DAT);
        if(gpio_value & (0x1<<gpio))
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}
#endif



static int check_wps_bt(void)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PABCD_DAT);
        if(gpio_value & WPS_BT_GPIO_BIT)
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}


int gpio_shadow;
int DIL_run_led_toggle(void)
{
        gpio_shadow = RTL_R32(PABCD_DAT);
//      printk("-> GPIO_DATA: %08x\n", gpio_shadow);
        if( gpio_shadow & RUN_LED_BIT)
                gpio_shadow &= ~RUN_LED_BIT;
        else
                gpio_shadow |= RUN_LED_BIT;
        RTL_W32(PABCD_DAT, gpio_shadow);
        return 0;
}



int DIL_run_led_on(void)
{
        gpio_shadow = RTL_R32(PABCD_DAT);
        gpio_shadow &= ~RUN_LED_BIT;
        RTL_W32(PABCD_DAT, gpio_shadow);
        return 0;
}

int DIL_run_led_off(void)
{
        gpio_shadow = RTL_R32(PABCD_DAT);
        gpio_shadow |= RUN_LED_BIT;
        RTL_W32(PABCD_DAT, gpio_shadow);
        return 0;
}

#endif
#else
static int check_reset_bt(void)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PEFGH_DAT);
        if(gpio_value & RESET_BT_GPIO_BIT)
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}


static int check_wps_bt(void)
{
        u32 gpio_value;

        gpio_value = RTL_R32(PEFGH_DAT);
        if(gpio_value & WPS_BT_GPIO_BIT)
                return 0; /* not pushed */
        else
                return 1; /* pushed */
}



int gpio_shadow;
int DIL_run_led_toggle(void)
{
        gpio_shadow = RTL_R32(PEFGH_DAT);
//      printk("-> GPIO_DATA: %08x\n", gpio_shadow);
        if( gpio_shadow & RUN_LED_BIT)
                gpio_shadow &= ~RUN_LED_BIT;
        else
                gpio_shadow |= RUN_LED_BIT;
        RTL_W32(PEFGH_DAT, gpio_shadow);
        return 0;
}


int DIL_run_led_on(void)
{
        gpio_shadow = RTL_R32(PEFGH_DAT);
        gpio_shadow &= ~RUN_LED_BIT;
        RTL_W32(PEFGH_DAT, gpio_shadow);
        return 0;
}

int DIL_run_led_off(void)
{
        gpio_shadow = RTL_R32(PEFGH_DAT);
        gpio_shadow |= RUN_LED_BIT;
        RTL_W32(PEFGH_DAT, gpio_shadow);
        return 0;
}
#endif
static int send_sig_pid( int sig, int pid)
{
        struct task_struct *p = NULL;

        p = find_task_by_vpid(1);
        return send_sig(sig,p,0);
}



#ifndef CONFIG_RTL_8198_GW
static int proc_write_usb_led( struct file *file, const char *buffer, u_long count, void *data )
{
	if(buffer[0]=='1')
	{
                gpio_shadow = RTL_R32(PABCD_DAT);
                gpio_shadow &= ~USB_LED_BIT;
                RTL_W32(PABCD_DAT, gpio_shadow);
printk("Write 1: gpio_shadow: %08x\n", gpio_shadow); 
	}
	else
	{
                gpio_shadow = RTL_R32(PABCD_DAT);
                gpio_shadow |= USB_LED_BIT;
                RTL_W32(PABCD_DAT, gpio_shadow);
printk("Write 0: gpio_shadow: %08x\n", gpio_shadow); 
	}

        return count;
}
#endif


int pushed_count, reset_start;
int use_wps_only_button;

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
int lanwan_sw_gpio = 11;
int lanwan_val=-1;

int extend_sw_gpio = 10;
int extend_val=-1;

int extender_led_gpio = 12;
int lan_led_gpio = 13;
int wan_led_gpio = 14;

int set_led_op(int gpio, int flag)
{
        gpio_shadow = RTL_R32(PABCD_DAT);
        if(flag) gpio_shadow &= ~(0x1 << gpio);
        else gpio_shadow |= (0x1 << gpio);
        RTL_W32(PABCD_DAT, gpio_shadow);
        return 0;
}

int blink_led(int gpio)
{
        gpio_shadow = RTL_R32(PABCD_DAT);
        if(gpio_shadow & (0x1<<gpio))
                gpio_shadow &= ~(0x1 <<gpio);
        else
                gpio_shadow |= (0x1 <<gpio);
        RTL_W32(PABCD_DAT, gpio_shadow);
        return 0;
}


int extender_led_op = RUN_LED_OFF;
int extender_led_operation(int period)
{
        if(extender_led_op == RUN_LED_BLINK)
                blink_led(extender_led_gpio);
        else if(extender_led_op == RUN_LED_OFF)
                set_led_op(extender_led_gpio,0);
        else
                set_led_op(extender_led_gpio,1);
}
#endif

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
#define HZ_DIVIDER 5
static int led_run;
#endif

static void rtl_gpio_timer(unsigned long data)
{

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        if(!led_run)
        {
                led_run = HZ_DIVIDER;
#endif
                if(run_blink == RUN_LED_BLINK)
                        DIL_run_led_toggle();
                else if(run_blink == RUN_LED_OFF)
                        DIL_run_led_off();
                else if(run_blink == RUN_LED_ON)
                        DIL_run_led_on();

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        }
        led_run--;

        extender_led_operation(run_period/HZ_DIVIDER);
#endif



        if(!reset_op_start)
        {
                mod_timer(&probe_timer, jiffies + run_period);
                return;
        }

        if(check_reset_bt())
        {
                pushed_count++;
#ifdef CONFIG_RTL8192CD
                if(!use_wps_only_button)
                {
                       if((pushed_count == 1) && !reset_start)
                       {
                               printk("Send WPS signal..\n");
                               run_blink=RUN_LED_ON;
                               send_sig_pid(SIGPWR, 1); /* send WPS signal */
                       }
                }
#endif
                /* Turn OFF Wireless LED */
                if(pushed_count == 7)
                {
                        pushed_count = 0;
                        reset_start = 1;
                        run_blink = RUN_LED_BLINK;
                        run_period = HZ/10;
                }
        }
        else
        {
                if(reset_start)
                {
                        printk("Send Reset signal..\n");
                        send_sig_pid( SIGPOLL , 1 );
                }

#ifdef CONFIG_RTL8192CD
//                if(pushed_count)
//                {
//                        printk("Send WPS signal..\n");
//                        send_sig_pid(SIGPWR, 1); /* send WPS signal */
//                }
#endif
                pushed_count = 0;
        }

#if	0
        if(use_wps_only_button && check_wps_bt())
        {
                printk("Send WPS signal..\n");
                run_blink=RUN_LED_ON;
                send_sig_pid(SIGPWR, 1); /* send WPS signal */
        }
#endif
#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        if(lanwan_sw_gpio != -1)
        {
                int cur_val;

                cur_val = check_bt(lanwan_sw_gpio);
                if(!check_bt(extend_sw_gpio))
                        cur_val = 1; /* always lan */

                if(lanwan_val == -1)
                        lanwan_val = cur_val;
                else
                {
                        if(lanwan_val != cur_val)
                        {
                                printk("Reboot for changing LAN/WAN Switch\n");
                                send_sig_pid(SIGTERM , 1 );
                                lanwan_val = cur_val;
                        }
                }
        }

        if(extend_sw_gpio != -1)
        {
                int cur_val;

                cur_val=check_bt(extend_sw_gpio);
                if(extend_val == -1)
                        extend_val = cur_val;
                else
                {
                        if(extend_val != cur_val)
                        {
                                printk("Reboot for changing Extend Switch\n");
                                send_sig_pid(SIGTERM , 1 );
                                extend_val = cur_val;
                        }
                }
        }
#endif

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        mod_timer(&probe_timer, jiffies + run_period/HZ_DIVIDER);
#else
        mod_timer(&probe_timer, jiffies + run_period);
#endif
}

#ifdef WPS_BT_GPIO_BIT
int led_run_count;
static void rtl_wps_timer(unsigned long data)
{
        led_run_count++;
        if(reset_start)
        {
#if	0
                ext_led_operation(EXT_2G_WLAN_GPIO,RUN_LED_FAST_BLINK,led_run_count);
                ext_led_operation(EXT_5G_WLAN_GPIO,RUN_LED_FAST_BLINK,led_run_count);
#endif
        }
        else
        {
		if(ext_2g_led_op != -1)
                	ext_led_operation(2,ext_2g_led_op,led_run_count);
		if(ext_5g_led_op != -1)
                	ext_led_operation(5,ext_5g_led_op,led_run_count);


                if(use_wps_only_button && check_wps_bt())
                {
                        printk("Send WPS signal..\n");
                        run_blink=RUN_LED_ON;
                        send_sig_pid(SIGPWR, 1); /* send WPS signal */
                }
        }
        mod_timer(&wps_timer, jiffies + HZ/10);
}
#endif


#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif

static int proc_write_run_led( struct file *file, const char *buffer, u_long count, void *data )
{
        if(!strncmp(buffer,"off",3))
        {
                DIL_run_led_off();
                run_blink = RUN_LED_OFF;
        }
        else if(!strncmp(buffer,"on",2))
        {
                run_blink = RUN_LED_ON;
        }
        else if(!strncmp(buffer,"blink",5))
        {
                run_blink = RUN_LED_BLINK;
        }

        if(check_reset_bt())
        {
                printk("Pushed..");
        }

        return count;
}


#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif
static int proc_read_run_period(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        p +=  sprintf(p, "%d", run_period );

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}

#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif
static int proc_read_reset_bt(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        if(!check_reset_bt())
                p +=  sprintf(p, "1");
        else
                p +=  sprintf(p, "0");

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}

static int proc_read_wps_bt(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        if(!check_wps_bt())
                p +=  sprintf(p, "1");
        else
                p +=  sprintf(p, "0");

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}



static int proc_write_reset_bt( struct file *file, const char *buffer, u_long count, void *data )
{
        if(!strncmp(buffer,"start",5))
        {
                reset_op_start=1;
                printk("Reset Button Start\n");
        }
        else if(!strncmp(buffer,"wps_function_off",5))
                use_wps_only_button = 1;

        return count;
}

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif
static int proc_read_lanwan_bt(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        if((lanwan_sw_gpio!= -1) && !check_bt(lanwan_sw_gpio))
        {
                if(!check_bt(extend_sw_gpio))
                        p +=  sprintf(p, "lan");
                else
                        p +=  sprintf(p, "wan");
        }
        else
                p +=  sprintf(p, "lan");

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}


static int proc_write_lanwan_bt( struct file *file, const char *buffer, u_long count, void *data )
{
        char buf2[32];

        memcpy(buf2,buffer,count);
        buf2[count]=0;
        lanwan_sw_gpio = simple_strtoul((const char*)(buf2), (char **)NULL, 10);
printk("lanwan_sw_gpio = %d\n", lanwan_sw_gpio );

        RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<lanwan_sw_gpio))));
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(0x1<<lanwan_sw_gpio))));

        return count;
}




static int proc_read_extend_bt(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        if((extend_sw_gpio!= -1) && !check_bt(extend_sw_gpio))
                p +=  sprintf(p, "extender");
        else
                p +=  sprintf(p, "ap");

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}


static int proc_write_extend_bt( struct file *file, const char *buffer, u_long count, void *data )
{
        char buf2[32];

        memcpy(buf2,buffer,count);
        buf2[count]=0;
        extend_sw_gpio = simple_strtoul((const char*)(buf2), (char **)NULL, 10);
printk("extend_sw_gpio = %d\n", extend_sw_gpio );

        RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<extend_sw_gpio))));
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(0x1<<extend_sw_gpio))));

        return count;
}


static int proc_write_wan_led( struct file *file, const char *buffer, u_long count, void *data )
{
printk("GPIO MUX2= %08x\n", RTL_R32(PIN_MUX_SEL_2));
        gpio_shadow = RTL_R32(PABCD_DAT);
        if(!strncmp(buffer,"off",3))
                gpio_shadow |= (0x1<<wan_led_gpio);
        else if(!strncmp(buffer,"on",2))
                gpio_shadow &= ~(0x1<<wan_led_gpio);
        RTL_W32(PABCD_DAT, gpio_shadow);

        return count;
}



static int proc_write_lan_led( struct file *file, const char *buffer, u_long count, void *data )
{
        gpio_shadow = RTL_R32(PABCD_DAT);
        if(!strncmp(buffer,"off",3))
                gpio_shadow |= (0x1<<lan_led_gpio);
        else if(!strncmp(buffer,"on",2))
                gpio_shadow &= ~(0x1<<lan_led_gpio);
        RTL_W32(PABCD_DAT, gpio_shadow);

        return count;
}


static int proc_write_extender_led( struct file *file, const char *buffer, u_long count, void *data )
{
        gpio_shadow = RTL_R32(PABCD_DAT);
        if(!strncmp(buffer,"off",3))
                gpio_shadow |= (0x1<<extender_led_gpio);
        else if(!strncmp(buffer,"on",2))
                gpio_shadow &= ~(0x1<<extender_led_gpio);
        RTL_W32(PABCD_DAT, gpio_shadow);

        if(!strncmp(buffer,"off",3))
        {
                set_led_op(extender_led_gpio,0);
                extender_led_op = RUN_LED_OFF;
        }
        else if(!strncmp(buffer,"on",2))
        {
                set_led_op(extender_led_gpio,1);
                extender_led_op = RUN_LED_ON;
        }
        else if(!strncmp(buffer,"blink",5))
        {
                extender_led_op = RUN_LED_BLINK;
        }
        return count;
}
#endif



#ifdef CONFIG_EFM_PATCH_FOR_MULTI
static int proc_read_ext2g_wps_bt(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        if(!check_ext2g_wps_bt())
                p +=  sprintf(p, "1");
        else
                p +=  sprintf(p, "0");

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}

static int proc_write_ext2g_wps_bt( struct file *file, const char *buffer, u_long count, void *data )
{
        if(!strncmp(buffer,"on",2))
        {
                ext_2g_led_op = RUN_LED_ON;
		ext2g_led_on();	
        }
        else if(!strncmp(buffer,"off",3))
        {
                ext_2g_led_op = RUN_LED_OFF;
		ext2g_led_off();	

        }
        else if(!strncmp(buffer,"fast",4))
        {
                ext_2g_led_op = RUN_LED_FAST_BLINK;
        }
        else if(!strncmp(buffer,"slow",4))
        {
                ext_2g_led_op = RUN_LED_SLOW_BLINK;
        }

        return count;
}

static int proc_read_ext5g_wps_bt(char *buffer, char **start, off_t offset, int length, int *eof, void *data)
{
        char *p = buffer;
        int len;

        if(!check_ext5g_wps_bt())
                p +=  sprintf(p, "1");
        else
                p +=  sprintf(p, "0");

        len = p - buffer;
        if( len <= offset+length ) *eof = 1;
        *start = buffer + offset;
        len -= offset;
        if( len > length ) len = length;
        if( len < 0 ) len = 0;

        return len;
}


static int proc_write_ext5g_wps_bt( struct file *file, const char *buffer, u_long count, void *data )
{
        if(!strncmp(buffer,"on",2))
        {
                ext_5g_led_op = RUN_LED_ON;
		ext5g_led_on();	
        }
        else if(!strncmp(buffer,"off",3))
        {
                ext_5g_led_op = RUN_LED_OFF;
		ext5g_led_off();	

        }
        else if(!strncmp(buffer,"fast",4))
        {
                ext_5g_led_op = RUN_LED_FAST_BLINK;
        }
        else if(!strncmp(buffer,"slow",4))
        {
                ext_5g_led_op = RUN_LED_SLOW_BLINK;
        }

        return count;
}

static int proc_write_ext_indication( struct file *file, const char *buffer, u_long count, void *data )
{
        if(!strncmp(buffer,"on",2))
	{
		printk("EXT INDICATION LED ON\n");
		ext_indication_led_on();
	}
	else
	{
		printk("EXT INDICATION LED OFF\n");
		ext_indication_led_off();
	}
        return count;
}

#endif





static  int init_gpio_mode(void)
{
#ifdef CONFIG_RTL_8881A
#define RTL_GPIO_MUX3 0xb800004C
        REG32(RTL_GPIO_MUX3) = (REG32(RTL_GPIO_MUX3) & (~(0xf<<8)))|(0x6<<8); /* SET GPIO FUNCTION FOR GPIO A4 */
#ifdef CONFIG_EFM_EXTENDER
        REG32(RTL_GPIO_MUX3) = (REG32(RTL_GPIO_MUX3)|0x6); /* SET GPIO FUNCTION FOR GPIO A2 */
#endif

        REG32(PIN_MUX_SEL) &=  ~(  (3<<10) | (3<<3) | (1<<15) );     /* SET GPIO FUNCTION FOR GPIO E2 */
        REG32(PIN_MUX_SEL) = (REG32(PIN_MUX_SEL) &(~(7<<7)))|(0x3<<7);  /* SET GPIO FUNCTION FOR GPIO E2 */

    	/* Set to GPIO , reset & run */
        RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~RESET_BT_GPIO_BIT)));
#ifdef CONFIG_EFM_EXTENDER
        RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~EXT_INDICATION_GPIO_BIT)));
#endif

        RTL_W32(PEFGH_CNR, (RTL_R32(PEFGH_CNR) & (~RUN_LED_BIT)));
#ifdef CONFIG_EFM_EXTENDER
        RTL_W32(PEFGH_CNR, (RTL_R32(PEFGH_CNR) & (~EXT_2G_WLAN_GPIO_BIT)));
        RTL_W32(PEFGH_CNR, (RTL_R32(PEFGH_CNR) & (~EXT_5G_WLAN_GPIO_BIT)));
        RTL_W32(PEFGH_CNR, (RTL_R32(PEFGH_CNR) & (~EXT_2G_GPIO_BIT)));
        RTL_W32(PEFGH_CNR, (RTL_R32(PEFGH_CNR) & (~EXT_5G_GPIO_BIT)));
#endif

        /* Set GPIO Direction */
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~RESET_BT_GPIO_BIT)));
#ifdef CONFIG_EFM_EXTENDER
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | EXT_INDICATION_GPIO_BIT));
#endif

        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) | RUN_LED_BIT));
#ifdef CONFIG_EFM_EXTENDER
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) | EXT_2G_WLAN_GPIO_BIT));
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) | EXT_5G_WLAN_GPIO_BIT));
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) & (~EXT_2G_GPIO_BIT)));
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) & (~EXT_5G_GPIO_BIT)));
#endif

#elif CONFIG_RTL_8198
        /* Set to GPIO , reset & run */
        RTL_W32(PEFGH_CNR, (RTL_R32(PEFGH_CNR) & (~RUN_LED_BIT) & (~RESET_BT_GPIO_BIT) & (~WPS_BT_GPIO_BIT)));

        /* Set GPIO Direction */
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) & (~RESET_BT_GPIO_BIT)));
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) & (~WPS_BT_GPIO_BIT)));
        RTL_W32(PEFGH_DIR, (RTL_R32(PEFGH_DIR) | RUN_LED_BIT));
#else
        /* Set to GPIO , reset & run */
        RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~RUN_LED_BIT) & (~RESET_BT_GPIO_BIT) & (~WPS_BT_GPIO_BIT)) & (~USB_LED_BIT) );

        /* Set GPIO Direction */
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~RESET_BT_GPIO_BIT)));
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~WPS_BT_GPIO_BIT)));
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | RUN_LED_BIT));
        RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | USB_LED_BIT));

        gpio_shadow = RTL_R32(PABCD_DAT);
        gpio_shadow |= USB_LED_BIT;
        RTL_W32(PABCD_DAT, gpio_shadow);
#endif

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        if(lanwan_sw_gpio != -1)
        {
                RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<lanwan_sw_gpio))));
                RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(0x1<<lanwan_sw_gpio))));
        }

        if(extend_sw_gpio!= -1)
        {
                RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<extend_sw_gpio))));
                RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(0x1<<extend_sw_gpio))));
        }


        if(extender_led_gpio!= -1)
        {
                RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<extender_led_gpio))));
                RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (0x1<<extender_led_gpio)));
                set_led_op(extender_led_gpio,0);
        }


        if(wan_led_gpio!= -1)
        {
                RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<wan_led_gpio))));
                RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (0x1<<wan_led_gpio)));
                set_led_op(wan_led_gpio,0);
        }

        if(lan_led_gpio!= -1)
        {
                RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(0x1<<lan_led_gpio))));
                RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | (0x1<<lan_led_gpio)));
                set_led_op(lan_led_gpio,0);
        }
#endif
}

#ifdef USE_IRQ_FOR_WPS 
static irqreturn_t wps_gpio_interrupt_isr(int irq, void *dev_instance, struct pt_regs *regs)
{
        int i;

        if(RTL_R32(PABCD_ISR) & WPS_BT_GPIO_BIT)
        {
                printk("WPS Interrupt..\n");
                for(i=0; i<1000;i++)
                {
                        if(!check_wps_bt()) break;
                }

                if(i==1000)
                {
                        printk("Send WPS signal.. \n");
                        run_blink=RUN_LED_ON;
                        send_sig_pid(SIGPWR, 1); /* send WPS signal */
                }
        }

        RTL_W32(PABCD_ISR, RTL_R32(PABCD_ISR));

        return IRQ_HANDLED;
}
#endif

#ifdef CONFIG_RTL_8198
#define GPIO_IRQ_NUM            (16+17) // GPIO_EFGH
#endif


static  int init_iptime_led_spec(void)
{
        char name[128];
        struct proc_dir_entry *proc_entry=NULL;

        printk("Init IPTIME GPIO: \n");
        init_gpio_mode();

        sprintf(name, "driver/reset_bt");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
        {
                proc_entry->read_proc=&proc_read_reset_bt;
                proc_entry->write_proc=&proc_write_reset_bt;
        }

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        sprintf(name, "driver/ext2g");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
	{
                proc_entry->read_proc=&proc_read_ext2g_wps_bt;
                proc_entry->write_proc=&proc_write_ext2g_wps_bt;
	}

	sprintf(name, "driver/ext5g");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
	{
                proc_entry->read_proc=&proc_read_ext5g_wps_bt;
                proc_entry->write_proc=&proc_write_ext5g_wps_bt;
	}

	sprintf(name, "driver/ext_indication");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
	{
                proc_entry->write_proc=&proc_write_ext_indication;
	}
#endif

        sprintf(name, "driver/wps_bt");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
                proc_entry->read_proc=&proc_read_wps_bt;


        sprintf(name, "driver/run_led");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
                proc_entry->write_proc=&proc_write_run_led;

#ifndef CONFIG_RTL_8198_GW
        sprintf(name, "driver/usb_led");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
                proc_entry->write_proc=&proc_write_usb_led;
#endif

#ifdef USE_IRQ_FOR_WPS 
       RTL_R32(PAB_IMR) |= (0x01 << (WPS_BT_GPIO)*2); // enable interrupt in falling-edge
       if (request_irq(16, wps_gpio_interrupt_isr, IRQF_SHARED, "wps_gpio", NULL)) {
                printk("gpio request_irq(%d) error!\n", 16);
        }
#endif

#if     0
        //sprintf(name, "driver/run_period");
        proc_entry = create_proc_entry("r1",0,0);
        if(proc_entry)
        {
                proc_entry->write_proc=&proc_write_run_period;
                proc_entry->read_proc=&proc_read_run_period;
        }
#endif

#ifdef CONFIG_EFM_PATCH_FOR_MULTI
        sprintf(name, "driver/lanwan_bt");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
        {
                proc_entry->read_proc=&proc_read_lanwan_bt;
                proc_entry->write_proc=&proc_write_lanwan_bt;
        }

        sprintf(name, "driver/extend_bt");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
        {
                proc_entry->read_proc=&proc_read_extend_bt;
                proc_entry->write_proc=&proc_write_extend_bt;
        }


        sprintf(name, "driver/lan_led");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
                proc_entry->write_proc=&proc_write_lan_led;

        sprintf(name, "driver/wan_led");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
                proc_entry->write_proc=&proc_write_wan_led;


        sprintf(name, "driver/extender_led");
        proc_entry = create_proc_entry(name,0,0);
        if(proc_entry)
                proc_entry->write_proc=&proc_write_extender_led;
#endif
}

#else
static void rtl_gpio_timer(unsigned long data)
{
	unsigned int pressed=1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	struct pid *pid;
#endif
#if defined(CONFIG_RTL_8196CS)
	update_pcie_status();
#endif

#ifdef CONFIG_RTL_8198B
	if ((RTL_R32(0xBB000110) & (1 << 23)))
#elif  defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)
	if ((RTL_R32(RESET_PIN_DATABASE) & (1 << RESET_BTN_PIN)))
#endif
	{
		pressed = 0;
#ifdef CONFIG_RTL_8198B
		//turn off reset LED
		//RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) | ((1 << RESET_LED_PIN))));
		REG32(0xBB000108) |= (1<<0); //CTRL_GPIO output 1, off
		//REG32(0xBB000104) |= (1<<28); //CTRL_GPIO output 1, off, for test
#elif !defined(CONFIG_RTL_819XD) && !defined(CONFIG_RTL_8196E) || defined(CONFIG_RTK_VOIP_BOARD)
		//turn off LED0
                #ifdef RESET_LED_NO
		#ifndef CONFIG_RTL_8196C_GW_MP
		RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) | ((1 << RESET_LED_PIN))));
		#endif
		#endif
#endif
	}
	else
	{
		DPRINTK("Key pressed %d!\n", probe_counter+1);
	}

#ifdef CONFIG_RTL_8198B
	if ((RTL_R32(0xBB000110) & (1 << 25)))
#else
	if (RTL_R32(AUTOCFG_PIN_DATABASE) & (1 << AUTOCFG_BTN_PIN))
#endif
	{
#ifdef USE_INTERRUPT_GPIO
		wps_button_push = 0;
#endif
	}else{
#ifdef USE_INTERRUPT_GPIO
		wps_button_push++;
#endif
	}

	if (probe_state == PROBE_NULL)
	{
		if (pressed)
		{
			probe_state = PROBE_ACTIVE;
			probe_counter++;
		}
		else
			probe_counter = 0;
	}
	else if (probe_state == PROBE_ACTIVE)
	{
		if (pressed)
		{
			probe_counter++;

			if ((probe_counter >=2 ) && (probe_counter <=PROBE_TIME))
			{
				DPRINTK("2-5 turn on led\n");
				//turn on LED0
#ifdef CONFIG_RTL_8198B
				//RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN))));
				REG32(0xBB000108) &= ~(1<<0); //CTRL_GPIO output 0, on
				//REG32(0xBB000104) &= ~(1<<28); //CTRL_GPIO output 0, on, for test
#else
			#ifdef RESET_LED_NO
			  #if defined(CONFIG_RTL_ULINKER)
			  	RTLWIFINIC_GPIO_write(4, 1);
			  #else
				RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN))));
			  #endif
            #endif
#endif
			}
			else if (probe_counter >= PROBE_TIME)
			{
				// sparkling LED0
				DPRINTK(">5 \n");

#ifdef CONFIG_RTL_8198B
				if (probe_counter & 1) {
					//RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) | ((1 << RESET_LED_PIN))));
					REG32(0xBB000108) |= (1<<0); //CTRL_GPIO output 1, off
					//REG32(0xBB000104) |= (1<<28); //CTRL_GPIO output 1, off, for test
				}
				else {
					//RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN))));
					REG32(0xBB000108) &= ~(1<<0); //CTRL_GPIO output 0, on
					//REG32(0xBB000104) &= ~(1<<28); //CTRL_GPIO output 0, on, for test
				}

#else
            #ifdef RESET_LED_NO
			  #if defined(CONFIG_RTL_ULINKER)
			  	if (probe_counter & 1)
					RTLWIFINIC_GPIO_write(4, 0);
				else
					RTLWIFINIC_GPIO_write(4, 1);
			  #else
				if (probe_counter & 1)
					RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) | ((1 << RESET_LED_PIN))));
				else
					RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN))));
			  #endif
            #endif
#endif
			}
		}
		else
		{
			#if defined(CONFIG_RTL865X_SC)
			if (probe_counter < 5)
			#else
			if (probe_counter < 2)
			#endif
			{
				probe_state = PROBE_NULL;
				probe_counter = 0;
				DPRINTK("<2 \n");
				#if defined(CONFIG_RTL865X_SC)
					ResetToAutoCfgBtn = 1;
				#endif
			}
			else if (probe_counter >= PROBE_TIME)
			{


				//reload default
				default_flag = '1';

				//kernel_thread(reset_flash_default, (void *)1, SIGCHLD);
				return;

			}
			else
			{
				DPRINTK("2-5 reset 1\n");
			#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
				kill_proc(1,SIGTERM,1);
			#else
				pid = get_pid(find_vpid(1));
				kill_pid(pid,SIGTERM,1);
			#endif
				DPRINTK("2-5 reset 2\n");
				//kernel_thread(reset_flash_default, 0, SIGCHLD);
				return;
			}
		}
	}

#ifdef AUTO_CONFIG
	if (AutoCfg_LED_Blink==1)
	{
		if (AutoCfg_LED_Toggle) {
#ifdef CONFIG_RTL_8198B
			//RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
			REG32(0xBB000108) |= (1<<0); //CTRL_GPIO output 1, off
			//REG32(0xBB000104) |= (1<<28); //CTRL_GPIO output 1, off, for test
#else
               #ifdef AUTOCFG_LED_NO
		#if defined(CONFIG_RTL_8196CS)
			RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | ((1 << AUTOCFG_LED_PIN)<<OUT_MASK1)));
		#elif defined(CONFIG_RTL_ULINKER)
			RTLWIFINIC_GPIO_write(4, 0);
		#else
			RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
		#endif
               #endif
#endif
		}
		else {
#ifdef CONFIG_RTL_8198B
			//RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
			REG32(0xBB000108) &= ~(1<<0); //CTRL_GPIO output 0, on
			//REG32(0xBB000104) &= ~(1<<28); //CTRL_GPIO output 0, on, for test
#else
                #ifdef AUTOCFG_LED_NO
			#if defined(CONFIG_RTL_8196CS)
			RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~((1 << AUTOCFG_LED_PIN)<<OUT_MASK1))));
			#elif defined(CONFIG_RTL_ULINKER)
			RTLWIFINIC_GPIO_write(4, 1);
			#else
			 RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
			#endif
			
             #endif
#endif
		}
				
		if(AutoCfg_LED_Slow_Blink)
		{
			if(AutoCfg_LED_Slow_Toggle)
				AutoCfg_LED_Toggle = AutoCfg_LED_Toggle;
			else
				AutoCfg_LED_Toggle = AutoCfg_LED_Toggle? 0 : 1;
			
			AutoCfg_LED_Slow_Toggle = AutoCfg_LED_Slow_Toggle? 0 : 1;
		}
		else
			AutoCfg_LED_Toggle = AutoCfg_LED_Toggle? 0 : 1;
		
	}
#endif


	mod_timer(&probe_timer, jiffies + HZ);

}
#endif

#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE

#define SYSTEM_CONTRL_DUMMY_REG 0xb8003504

int is_bank2_root()
{
	//boot code will steal System's dummy register bit0 (set to 1 ---> bank2 booting
	//for 8198 formal chip 
	if ((RTL_R32(SYSTEM_CONTRL_DUMMY_REG)) & (0x00000001))  // steal for boot bank idenfy
		return 1;

	return 0;
}
static int read_bootbank_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag='1';

	if (is_bank2_root())  // steal for boot bank idenfy
		flag='2';
		
	len = sprintf(page, "%c\n", flag);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
}
#endif

#ifdef AUTO_CONFIG
#if defined(USE_INTERRUPT_GPIO)
static irqreturn_t gpio_interrupt_isr(int irq, void *dev_instance, struct pt_regs *regs)
{

	printk("%s %d\n",__FUNCTION__ , __LINE__);
	unsigned int status;

	status = REG32(PABCD_ISR);

	if((status & BIT(3)) != 0)
	{
		wps_button_push = 1; 		
		RTL_W32(PABCD_ISR, BIT(3)); 	
#ifdef IMMEDIATE_PBC
	if(wscd_pid>0)
	{
		rcu_read_lock();
		wscd_pid_Ptr = get_pid(find_vpid(wscd_pid));
		rcu_read_unlock();	

		if(wscd_pid_Ptr){
			printk("(%s %d);signal wscd ;pid=%d\n",__FUNCTION__ , __LINE__,wscd_pid);			
			kill_pid(wscd_pid_Ptr, SIGUSR2, 1);
			
		}

	}
#endif
	}
#ifdef CONFIG_RTK_VOIP
	wps_button_push = 1; 
	RTL_W32(PEFGH_ISR, RTL_R32(PEFGH_ISR)); 	
#endif
	return IRQ_HANDLED;
}
#endif

static int read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;

#if  defined(USE_INTERRUPT_GPIO)
// 2009-0414		
	if (wps_button_push) {
		flag = '1';
		//wps_button_push = 0; //mark it for select wlan interface by button pressed time		
	}
	else{
		if (RTL_R32(AUTOCFG_PIN_DATABASE) & (1 << AUTOCFG_BTN_PIN)){
			flag = '0';
		}else{
			//printk("wps button be held \n");
			flag = '1';
		}

	}
// 2009-0414		
#else

#ifdef CONFIG_RTL_8198B
	if ((RTL_R32(0xBB000110) & (1 << 25)))
#else
	if (RTL_R32(AUTOCFG_PIN_DATABASE) & (1 << AUTOCFG_BTN_PIN))
#endif
		flag = '0';
	else {
		flag = '1';
	}
#endif // CONFIG_RTL865X_KLD		
		
	len = sprintf(page, "%c\n", flag);


	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
}


#if defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)
static int usb_mode_detect_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;
	#if !defined(CONFIG_RTK_VOIP_BOARD)
	#if defined(CONFIG_RTL_8197D)
	if(RTL_R32(USB_MODE_DETECT_PIN_IOBASE) & (1 << (USB_MODE_DETECT_PIN_NO)))
	{
		flag = '1';
	}
	else
	{
		flag = '0';
	}	
	#elif (defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8196E)) && defined(CONFIG_RTL8192CD)
	if(get_8192cd_gpio0_7()& (1 << (USB_MODE_DETECT_PIN_NO)))
	{
		flag = '1';
	}
	else
	{
		flag = '0';
	}	
	#endif
	#endif
	len = sprintf(page, "%c\n", flag);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
}
#endif

#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif 
static int write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[20];
//Brad add for update flash check 20080711

	char start_count[10], wait[10];

	if (count < 2)
		return -EFAULT;

	DPRINTK("file: %08x, buffer: %s, count: %lu, data: %08x\n",
		(unsigned int)file, buffer, count, (unsigned int)data);

	if (buffer && !copy_from_user(&flag, buffer, 1)) {
		if (flag[0] == 'E') {
			autoconfig_gpio_init();
			#ifdef CONFIG_RTL865X_CMO
			extra_led_gpio_init();
			#endif
		}
		else if (flag[0] == '0')
			autoconfig_gpio_off();
		else if (flag[0] == '1')
			autoconfig_gpio_on();
		else if (flag[0] == '2')
			autoconfig_gpio_blink();
#ifndef CONFIG_RTL_8196CS
		else if (flag[0] == '3')
			autoconfig_gpio_slow_blink();
#endif

#ifdef CONFIG_RTL_8196C_GW_MP
		else if (flag[0] == '9') // disable system led
                {
			all_led_on();
		}
#endif	

#ifdef CONFIG_RTL865X_CMO
		else if (flag[0] == '3')
			wep_wpa_led_on();
		else if (flag[0] == '5')
			wep_wpa_led_off();
		else if (flag[0] == '6')
			mac_ctl_led_on();
		else if (flag[0] == '7')
			mac_ctl_led_off();
		else if (flag[0] == '8')
			bridge_repeater_led_on();
		else if (flag[0] == '9')
			bridge_repeater_led_off();
		else if (flag[0] == 'A')
			system_led_on();
//		else if (flag[0] == 'B')
//			system_led_off();
		else if (flag[0] == 'C')
			lan_led_on();
		else if (flag[0] == 'D')
			lan_led_off();
		else if (flag[0] == 'Z')
			printk("gpio test test\n");
//		else if (flag[0] == '9')
//			system_led_blink = 2;
#endif

//Brad add for update flash check 20080711

		else if (flag[0] == '4'){
			start_count_time= 1;
			sscanf(buffer, "%s %s", start_count, wait);
			Reboot_Wait = (simple_strtol(wait,NULL,0))*100;
		}


		else
			{}

		return count;
	}
	else
		return -EFAULT;
}
#ifdef IMMEDIATE_PBC
static unsigned long atoi_dec(char *s)
{
	unsigned long k = 0;

	k = 0;
	while (*s != '\0' && *s >= '0' && *s <= '9') {
		k = 10 * k + (*s - '0');
		s++;
	}
	return k;
}
static int read_gpio_wscd_pid(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;

	DPRINTK("wscd_pid=%d\n",wscd_pid);	
	
	len = sprintf(page, "%d\n", wscd_pid);
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
}
static int write_gpio_wscd_pid(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[20];
	char start_count[10], wait[10];
	if (count < 2)
		return -EFAULT;

	DPRINTK("file: %08x, buffer: %s, count: %lu, data: %08x\n",
		(unsigned int)file, buffer, count, (unsigned int)data);

	if (buffer && !copy_from_user(&flag, buffer, 1)) {

		wscd_pid = atoi_dec(buffer);
		DPRINTK("wscd_pid=%d\n",wscd_pid);	
		return count;
	}
	else{
		return -EFAULT;
	}
}
#endif
#endif // AUTO_CONFIG

static int default_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%c\n", default_flag);
	if (len <= off+count) *eof = 1;
	  *start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	  return len;
}

#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif 
static int default_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	if (count < 2)
		return -EFAULT;
	if (buffer && !copy_from_user(&default_flag, buffer, 1)) {
		return count;
	}
	return -EFAULT;
}

#ifdef READ_RF_SWITCH_GPIO
static int rf_switch_read_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;

	if (RTL_R32(WIFI_ONOFF_PIN_DATABASE) & (1<<WIFI_ONOFF_PIN_NO)){
		flag = '1';
	}else{
		flag = '0';
	}
	len = sprintf(page, "%c\n", flag);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
}
#endif

#ifdef CONFIG_POCKET_ROUTER_SUPPORT
static int write_pocketAP_hw_set_flag_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	unsigned int reg_cnr, reg_dir;

	if (count != 2)
		return -EFAULT;
	if (buffer && !copy_from_user(&pocketAP_hw_set_flag, buffer, 1)) {

	}
	return -EFAULT;
}

static int read_pocketAP_hw_set_flag_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	
	len = sprintf(page, "%c\n", pocketAP_hw_set_flag);
	
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

//panic_printk("\r\n __[%s-%u]",__FILE__,__LINE__);	
	return len;

}



static int read_ap_client_rou_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;
	int gpioA2_flag,gpioB3_flag;
	
	
	if(ap_cli_rou_state == 2)
	{
		len = sprintf(page, "%c\n", '2'); // AP
	}
	else if(ap_cli_rou_state == 1)
	{
		len = sprintf(page, "%c\n", '1'); // Client
	}
	else if(ap_cli_rou_state == 3)
	{
		len = sprintf(page, "%c\n", '3'); // Router
	}
	else
	{
		len = sprintf(page, "%c\n", '0'); 
	}
	
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

//panic_printk("\r\n __[%s-%u]",__FILE__,__LINE__);	
	return len;
}

static int read_dc_pwr_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;
	int pluged_state=0;
//panic_printk("\r\n 0x%x__[%s-%u]",RTL_R32(RESET_PIN_DATABASE),__FILE__,__LINE__);		

	pluged_state = get_dc_pwr_plugged_state();
	if(pluged_state == 1)
	{
		len = sprintf(page, "%c\n", '1');
	}
	else if(pluged_state == 2)
	{
		len = sprintf(page, "%c\n", '2');
	}
	else
	{
		len = sprintf(page, "%c\n", '0');
	}		

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

//panic_printk("\r\n len=[%u]__[%s-%u]",len,__FILE__,__LINE__);	
	return len;
}

static int read_dc_pwr_plugged_flag_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	
	len = sprintf(page, "%c\n", dc_pwr_plugged_flag);
	
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

//panic_printk("\r\n __[%s-%u]",__FILE__,__LINE__);
	dc_pwr_plugged_flag = '0';
	return len;

}
static int read_EnablePHYIf_proc(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;

//panic_printk("\r\n 0x%x__[%s-%u]",RTL_R32(RESET_PIN_DATABASE),__FILE__,__LINE__);		

	if(RTL_R32(0xBB804114) & (0x01))
	{
		flag = '1';
	}
	else
	{
		flag = '0';
	}
		
	len = sprintf(page, "%c\n", flag);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

//panic_printk("\r\n len=[%u]__[%s-%u]",len,__FILE__,__LINE__);	
	return len;
}

static int get_pocketAP_ap_cli_rou_state()
{
	int gpioA2_flag,gpioB3_flag;
	
	if(RTL_R32(RESET_PIN_DATABASE) & (RTL_GPIO_DAT_GPIOA2))
	{
		gpioA2_flag = 1;
	}
	else
	{
		gpioA2_flag = 0;
	}

	if(RTL_R32(RESET_PIN_DATABASE) & (RTL_GPIO_DAT_GPIOB3))
	{
		gpioB3_flag = 1;
	}
	else
	{
		gpioB3_flag = 0;
	}

	return ((1<<gpioA2_flag)|gpioB3_flag);
}

static int get_dc_pwr_plugged_state()
{
	
	if(RTL_R32(RESET_PIN_DATABASE) & (RTL_GPIO_DAT_GPIOC0))
	{
		return 1; //plugged
	}
	else
	{
		return 2; //unplugged
	}

}

static int check_EnablePHYIf()
{
	if(RTL_R32(0xBB804114) & (0x01))
	{
		return 1;
	}
	else
	{
		return 0;
	}

}

static void pocket_ap_timer_func(unsigned long data)
{
//panic_printk("\r\n __[%s-%u]",__FILE__,__LINE__);

	if(ap_cli_rou_idx >= 1)
		ap_cli_rou_idx = 0;
	else
		ap_cli_rou_idx++;

	ap_cli_rou_time_state[ap_cli_rou_idx]=get_pocketAP_ap_cli_rou_state();
	dc_pwr_plugged_time_state = get_dc_pwr_plugged_state();

	if(ap_cli_rou_time_state[0] == ap_cli_rou_time_state[1] )
	{
		if(ap_cli_rou_state != ap_cli_rou_time_state[0])
		{
			ap_cli_rou_state = ap_cli_rou_time_state[0];
			pocketAP_hw_set_flag = '0';
		}
	}

	if(dc_pwr_plugged_state == 0)
	{
		dc_pwr_plugged_state = dc_pwr_plugged_time_state;
	}
	else if(dc_pwr_plugged_state != dc_pwr_plugged_time_state)
	{
		dc_pwr_plugged_state = dc_pwr_plugged_time_state;
		dc_pwr_plugged_flag = '1';
	}
	
//B8b00728 & 0x1F 0x11:L0 0x14:L1  
//panic_printk("\r\n [%d-%d-%d-%d],__[%s-%u]",ap_cli_rou_time_state[0],ap_cli_rou_time_state[1],ap_cli_rou_state,__FILE__,__LINE__);		

//panic_printk("\r\n [0x%x]",RTL_R32(0xB8b00728) & (0x1F));
	pwr_saving_state=(RTL_R32(0xB8b00728) & (0x1F));
//panic_printk("\r\n pwr_saving_state = [0x%x]",pwr_saving_state);

	if(pwr_saving_state == 0x14) // L1 state, in low speed
	{
		if (pwr_saving_led_toggle < 2) {
			RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) | (RTL_GPIO_DAT_GPIOB2)));
			pwr_saving_led_toggle++;
		}
		else if (pwr_saving_led_toggle < 4){
			RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(RTL_GPIO_DAT_GPIOB2))));
			pwr_saving_led_toggle++;
			if(pwr_saving_led_toggle == 4)
				pwr_saving_led_toggle = 0;
		}
		else
		{
			pwr_saving_led_toggle = 0;
		}
	}
	else // L0 state, always on
	{
		RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(RTL_GPIO_DAT_GPIOB2))));
	}


	mod_timer(&pocket_ap_timer, jiffies + HZ/2);
}
#endif



#if defined(CONFIG_RTL_ULINKER)
static int pre_status = -1;
static int nxt_status = -1;
static int cur_status = -1;
static int switched   = 0;
static int running    = 0;

//static char ulinker_ap_cl_flag = '0'; /* 0: client, 1: router */
static char ulinker_ap_cl_flag = '1'; /* 0: client, 1: router */

static int read_ulinker_ap_cl_switching(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%d\n", running);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

	return len;
}

static int write_ulinker_ap_cl_switching(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp = '0';

	if (count != 2)
		return -EFAULT;
	if (buffer && !copy_from_user(&tmp, buffer, 1)) {
		if (tmp == '0') {
			running = 0;
		}
	}
	return -EFAULT;
}

static int read_ulinker_ap_cl(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%c\n", ulinker_ap_cl_flag);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

	return len;
}

#if defined(CONFIG_RTL_ULINKER_WLAN_DELAY_INIT)
static struct proc_dir_entry *res_wlan=NULL;
int wlan_init_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
	unsigned char tmp[20];

	if (count < 2 || count > 20)
	return -EFAULT;

	memset(tmp, '\0', 20);
	if (buf && !copy_from_user(tmp, buf, count))
	{
		tmp[count-1]=0;

		if(!strcmp(tmp, "wlan 1"))
		{
			extern int rtl8192cd_init(void);
			rtl8192cd_init();
		}
	#if 0 //our wlan driver can't unreg...
		else if (!strcmp(tmp, "wlan 0"))
		{
			extern void rtl8192cd_exit (void);
			printk("wlan drv unreg !\n");
			rtl8192cd_exit();
		}
	#endif
	}

	return count;
}
#endif

static void ulinker_timer_func(unsigned long data)
{
	//panic_printk("rtl_gpio: cur_status[%d], ulinker_ap_cl_flag[%c]\n", cur_status, ulinker_ap_cl_flag);
	if (running == 0)
	{
		cur_status = (RTL_R32(RESET_PIN_DATABASE) & (RTL_GPIO_DAT_GPIOA6));
		//panic_printk("rtl_gpio: cur_status[%d]\n", cur_status);

		if (pre_status == -1) {
			pre_status = cur_status;

			if (cur_status == 0)
				ulinker_ap_cl_flag = '1'; /* router */
			else
				ulinker_ap_cl_flag = '0'; /* client */
		}
		else if (pre_status != cur_status || switched) {
			if (switched == 0) {
				switched =1;
				nxt_status = cur_status;
			}
			else {
				if (cur_status == nxt_status)
					switched++;
				else {
					//panic_printk("rtl_gpio: clean\n");
					switched = 0;
				}

				if (switched > 2) {
					//panic_printk("rtl_gpio: change mode [%d->%d]\n", pre_status, cur_status);
					running  = 1;
					switched = 0;
					pre_status = cur_status;
					nxt_status = -1;

					if (cur_status == 0)
						ulinker_ap_cl_flag = '1'; /* router */
					else
						ulinker_ap_cl_flag = '0'; /* client */
				}
			}
		}
		else {
			pre_status = cur_status;
		}
	}

	mod_timer(&ulinker_timer, jiffies + HZ/2);
}

enum {
	RTL_GADGET_FSG,
	RTL_GADGET_ETH,
};

int rtl_otg_gadget = RTL_GADGET_FSG;

int otg_proc_read_gadget(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int len = 0;
	len += sprintf(buf,"%d", rtl_otg_gadget);
	return len;
}

int otg_proc_read_wall_mount(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	extern int wall_mount;
	int len = 0;
	len += sprintf(buf,"%d", wall_mount);
	return len;
}

int otg_proc_read_cdc_rndis_status(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	extern int wall_mount;
	int len = 0;
	int cdc_rndis_status;

	if (wall_mount == 1)
		cdc_rndis_status = 0;
	else
		cdc_rndis_status = 1;

	len += sprintf(buf,"%d", cdc_rndis_status);
	return len;
}

int otg_proc_read_ether_state(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	extern int ether_state;
	int len = 0;
	len += sprintf(buf,"%d", ether_state);
	return len;
}

int otg_proc_read_fsg_state(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	extern unsigned long fsg_jiffies;
	extern unsigned long rst_jiffies;

	int len = 0;
	if (rst_jiffies==0)
		len += sprintf(buf,"%d", 0);
	else
		len += sprintf(buf,"%d", (rst_jiffies-fsg_jiffies)/HZ);
	return len;
}

int otg_proc_write_gadget(struct file *file, const char *buf, unsigned long count, void *data)
{
	extern void dwc_otg_driver_cleanup(void);
	extern int dwc_otg_driver_init(void);

	unsigned char tmp[20];

	if (count < 2 || count > 20)
		return -EFAULT;

	memset(tmp, '\0', 20);
	if (buf && !copy_from_user(tmp, buf, count))
	{
		tmp[count-1]=0;

		//if (rtl_otg_gadget == RTL_GADGET_ETH)
			//return count;
	#if 1
		if(!strcmp(tmp, "gadget fsg"))
		{
			dwc_otg_driver_cleanup();
			rtl_otg_gadget = RTL_GADGET_FSG;
			dwc_otg_driver_init();
		}
		else
	#endif
		if (!strcmp(tmp, "gadget eth"))
		{
			dwc_otg_driver_cleanup();
			rtl_otg_gadget = RTL_GADGET_ETH;
			dwc_otg_driver_init();
		}
	}

	return count;
}


char ulinker_rndis_mac[20];
static int read_ulinker_rndis_mac(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%s\n", ulinker_rndis_mac);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;

	return len;
}
static int write_ulinker_rndis_mac(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	if (count < 12)
		return -EFAULT;
	if (buffer && !copy_from_user(&ulinker_rndis_mac, buffer, 12)) {
		//panic_printk("[%s:%d] ulinker_rndis_mac[%s]\n", __FUNCTION__, __LINE__, ulinker_rndis_mac);
	}
	return -EFAULT;
}

//extern int PCIE_reset_procedure(int portnum, int Use_External_PCIE_CLK, int mdio_reset);
extern int PCIE_reset_procedure(int PCIE_Port0and1_8196B_208pin, int Use_External_PCIE_CLK, int mdio_reset,unsigned long conf_addr);
int PCIE_Host_Init(int argc, char* argv[])
{
	int portnum=0;
	if(argc >= 1) 
	{	portnum= simple_strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	}

#define PCIE0_RC_CFG_BASE (0xb8b00000)
#define PCIE0_RC_EXT_BASE (PCIE0_RC_CFG_BASE + 0x1000)
#define PCIE0_EP_CFG_BASE (0xb8b10000)

#define PCIE1_RC_CFG_BASE (0xb8b20000)
#define PCIE1_RC_EXT_BASE (PCIE1_RC_CFG_BASE + 0x1000)
#define PCIE1_EP_CFG_BASE (0xb8b30000)

#define PCIE0_MAP_IO_BASE  (0xb8c00000)
#define PCIE0_MAP_MEM_BASE (0xb9000000)

#define PCIE1_MAP_IO_BASE  (0xb8e00000)
#define PCIE1_MAP_MEM_BASE (0xba000000)

#define MAX_READ_REQSIZE_128B    0x00
#define MAX_READ_REQSIZE_256B    0x10
#define MAX_READ_REQSIZE_512B    0x20
#define MAX_READ_REQSIZE_1KB     0x30
#define MAX_READ_REQSIZE_2KB     0x40
#define MAX_READ_REQSIZE_4KB     0x50

#define MAX_PAYLOAD_SIZE_128B    0x00
#define MAX_PAYLOAD_SIZE_256B    0x20
#define MAX_PAYLOAD_SIZE_512B    0x40
#define MAX_PAYLOAD_SIZE_1KB     0x60
#define MAX_PAYLOAD_SIZE_2KB     0x80
#define MAX_PAYLOAD_SIZE_4KB     0xA0

	int rc_cfg, cfgaddr;
	int iomapaddr;
	int memmapaddr;

	if(portnum==0)
	{	rc_cfg=PCIE0_RC_CFG_BASE;
		cfgaddr=PCIE0_EP_CFG_BASE;
		iomapaddr=PCIE0_MAP_IO_BASE;
		memmapaddr=PCIE0_MAP_MEM_BASE;
	}
	else if(portnum==1)
	{	rc_cfg=PCIE1_RC_CFG_BASE;
		cfgaddr=PCIE1_EP_CFG_BASE;
		iomapaddr=PCIE1_MAP_IO_BASE;
		memmapaddr=PCIE1_MAP_MEM_BASE;	
	}
	else 
	{	return 0;
	}
	
	int t=REG32(rc_cfg);
	unsigned int vid=t&0x0000ffff;   //0x819810EC
	unsigned int pid=(t&0xffff0000) >>16;
	
	if( (vid!= 0x10ec) || (pid!=0x8196))
	{	//DBG_PRINT("VID=%x, PID=%x \n", vid, pid );
		//DBG_PRINT(" !!!  Read VID PID fail !!! \n");
		//at_errcnt++;
		return;
	}

	//STATUS
	//bit 4: capabilties List

	//CMD
	//bit 2: Enable Bys master, 
	//bit 1: enable memmap, 
	//bit 0: enable iomap

	REG32(rc_cfg + 0x04)= 0x00100007;   

	//Device Control Register 
	//bit [7-5]  payload size
	REG32(rc_cfg + 0x78)= (REG32(rc_cfg + 0x78 ) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B;  // Set MAX_PAYLOAD_SIZE to 128B,default
	  
      REG32(cfgaddr + 0x04)= 0x00100007;    //0x00180007

	//bit 0: 0:memory, 1 io indicate
      REG32(cfgaddr + 0x10)= (iomapaddr | 0x00000001) & 0x1FFFFFFF;  // Set BAR0

	//bit 3: prefetch
	//bit [2:1] 00:32bit, 01:reserved, 10:64bit 11:reserved
      REG32(cfgaddr + 0x18)= (memmapaddr | 0x00000004) & 0x1FFFFFFF;  // Set BAR1  


	//offset 0x78 [7:5]
      REG32(cfgaddr + 0x78) = (REG32(cfgaddr + 0x78) & (~0xE0)) | (MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B

	//offset 0x79: [6:4] 
      REG32(cfgaddr + 0x78) = (REG32(cfgaddr + 0x78) & (~0x7000)) | (MAX_READ_REQSIZE_256B<<8);  // Set MAX_REQ_SIZE to 256B,default

	  
	//check
//      if(REG32(cfgaddr + 0x10) != ((iomapaddr | 0x00000001) & 0x1FFFFFFF))
      {	//at_errcnt++;
      		//DBG_PRINT("Read Bar0=%x \n", REG32(cfgaddr + 0x10)); //for test
      	}
	  

//	if(REG32(cfgaddr + 0x18)!=((memmapaddr| 0x00000004) & 0x1FFFFFFF))
	{	//at_errcnt++;
      		//DBG_PRINT("Read Bar1=%x \n", REG32(cfgaddr + 0x18));      //for test
	}
	//DBG_PRINT("Set BAR finish \n");


	//io and mem limit, setting to no litmit
	REG32(rc_cfg+ 0x1c) = (2<<4) | (0<<12);   //  [7:4]=base  [15:12]=limit
	REG32(rc_cfg+ 0x20) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit	
	REG32(rc_cfg+ 0x24) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit		


#undef PCIE0_RC_EXT_BASE
#undef PCIE0_EP_CFG_BASE

#undef PCIE1_RC_CFG_BASE
#undef PCIE1_RC_EXT_BASE
#undef PCIE1_EP_CFG_BASE

#undef PCIE0_MAP_IO_BASE
#undef PCIE0_MAP_MEM_BASE

#undef PCIE1_MAP_IO_BASE
#undef PCIE1_MAP_MEM_BASE

#undef MAX_READ_REQSIZE_128B
#undef MAX_READ_REQSIZE_256B
#undef MAX_READ_REQSIZE_512B
#undef MAX_READ_REQSIZE_1KB
#undef MAX_READ_REQSIZE_2KB
#undef MAX_READ_REQSIZE_4KB

#undef MAX_PAYLOAD_SIZE_128B
#undef MAX_PAYLOAD_SIZE_256B
#undef MAX_PAYLOAD_SIZE_512B
#undef MAX_PAYLOAD_SIZE_1KB
#undef MAX_PAYLOAD_SIZE_2KB
#undef MAX_PAYLOAD_SIZE_4KB
}

spinlock_t sysled_lock = SPIN_LOCK_UNLOCKED;
static int sysled_toggle = 1;
static struct timer_list sysled_timer;
extern void renable_sw_LED(void);

void system_led_control(unsigned long data)
{
#define PCIE0_EP_CFG_BASE (0xb8b10000)

	static int on = 0;
	unsigned int reg;

	if (sysled_toggle == 1) {
		reg = (REG32(PCIE0_EP_CFG_BASE + 0x18) & 0xffff0000) | 0xb0000000;
		if (on == 1) {
			(*(volatile u32 *)(reg + 0x44)) = 0x10100000;
			on = 0;
		}
		else {
			(*(volatile u32 *)(reg + 0x44)) = 0x00000000;
			on = 1;
		}

		mod_timer(&sysled_timer, jiffies + HZ/2);
	}

#undef PCIE0_EP_CFG_BASE
}

void system_led_init()
{
#define PCIE0_EP_CFG_BASE (0xb8b10000)

	char *arg_v[] = {"hinit", "0"};
	unsigned int reg;
	static int init = 0;

	if (init == 0) {
		spin_lock_init(&sysled_lock);

                PCIE_reset_procedure(0, 0, 1,PCIE0_EP_CFG_BASE);
		PCIE_Host_Init(2, arg_v);
		reg = (REG32(PCIE0_EP_CFG_BASE + 0x18) & 0xffff0000) | 0xb0000000;	
		(*(volatile u32 *)(reg + 0x44)) = 0x30300000;

		init_timer(&sysled_timer);
		sysled_timer.data = (unsigned long)NULL;
		sysled_timer.function = &system_led_control;
		mod_timer(&sysled_timer, jiffies + HZ/2);
	}
#undef PCIE0_EP_CFG_BASE
}


static int write_ulinker_led(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp[16];

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 8)) {
		if (tmp[0] == '0'){
			sysled_toggle = 0;
			RTLWIFINIC_GPIO_write(4, 0);
			renable_sw_LED();
		}
		else if (tmp[0] == '1'){
			sysled_toggle = 1;
			mod_timer(&sysled_timer, jiffies + HZ/2);
		}

		return count;
	}
	return -EFAULT;
}
#endif /* #if defined(CONFIG_RTL_ULINKER) */

static int write_watchdog_reboot(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp[16];

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 8)) {	
		if (tmp[0] == '1') {
			local_irq_disable();	
			panic_printk("reboot...\n");
			*(volatile unsigned long *)(0xB800311c)=0; /*this is to enable 865xc watch dog reset*/
			for(;;);
		}

		return count;
	}
	return -EFAULT;
}

int __init rtl_gpio_init(void)
{
	struct proc_dir_entry *res=NULL;

#if defined(CONFIG_RTL_ULINKER)
	system_led_init();
#endif

	printk("Realtek GPIO Driver for Flash Reload Default\n");

#ifdef CONFIG_RTL_8198B
	//gpio65, system(reset)/WPS led
	REG32(0xBB000120) |= (1<<0); //EN_GPIO
	REG32(0xBB0001DC) |= (1<<0); //SEL_GPIO select output mode

	//gpio56, reset/default button
	REG32(0xBB00011C) |= (1<<23); //EN_GPIO
	REG32(0xBB0001D8) &= ~(1<<23); //SEL_GPIO select input mode
	
	res = create_proc_entry("gpio", 0, NULL);
	if (res) {
		res->read_proc = read_proc;
		res->write_proc = write_proc;
	}
	else {
		printk("Realtek GPIO Driver, create proc failed!\n");
	}
#elif defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E)
	#if defined(CONFIG_RTL_8881A)

	    RTL_W32((PIN_MUX_SEL+0xC), ((RTL_R32(PIN_MUX_SEL+0xC)&(~0xFFFFF))|(0x00066666) ));
		//panic_printk("===>%x\r\n",RTL_R32(PIN_MUX_SEL+0xC));
	#endif
	#if defined(CONFIG_RTK_VOIP_BOARD)
		RTL_W32(RTL_GPIO_MUX, (RTL_R32(RTL_GPIO_MUX) | (RTL_GPIO_MUX_DATA))); 
		#ifdef RESET_LED_NO
		RTL_W32(RESET_LED_IOBASE, (RTL_R32(RESET_LED_IOBASE) | (((1 << RESET_LED_PIN)))));
		RTL_W32(RESET_LED_DIRBASE, (RTL_R32(RESET_LED_DIRBASE) | ((1 << RESET_LED_PIN))));
		#endif
		RTL_W32(RESET_PIN_IOBASE,  (RTL_R32(RESET_PIN_IOBASE)  & (~(1 << RESET_BTN_PIN))));
		RTL_W32(RESET_PIN_DIRBASE, (RTL_R32(RESET_PIN_DIRBASE) & (~(1 << RESET_BTN_PIN))));
	#elif defined(CONFIG_RTL_8197D)
		//reg_iocfg_jtag config as gpio mode,gpioA[0~1], gpioA[2~6]
		#ifdef CONFIG_USING_JTAG
		RTL_W32(PIN_MUX_SEL, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_GPIOA0_1));
		#else
		RTL_W32(PIN_MUX_SEL, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_POCKETAP_DATA));
		#endif
		//reg_iocfg_led_p1, reg_iocfg_led_p2, config as gpio mode,GPIOB[7], GPIOC[0]
		RTL_W32(PIN_MUX_SEL_2, (RTL_R32(PIN_MUX_SEL_2) | RTL_GPIO_MUX_2_POCKETAP_DATA));  	
		//set gpioA[1~6],gpioB[7],gpioC[0] as GPIO PIN
		RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & ~(RTL_GPIO_CNR_POCKETAP_DATA)));	
		//set direction, GPIOA[1,3,4,5] INPUT, GPIOA[2,6] OUTPUT

#ifdef CONFIG_RTL_8367R_SUPPORT
		// for 8367r h/w reset pin
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOA1))));
		RTL_W32(PABCD_DAT, (RTL_R32(PABCD_DAT) & (~(RTL_GPIO_DAT_GPIOA1))));
#else		
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA1))));
#endif


		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA3))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA4))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA5))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOA2))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOA6))));
		//set direction, GPIOB[7] INPUT, GPIOC[0] OUTPUT
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOB7))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOC0))));
	#elif defined(CONFIG_RTL_8197DL)
		//reg_iocfg_jtag config as gpio mode,gpioA[0~1], gpioA[2~6]
		#ifdef CONFIG_USING_JTAG
		RTL_W32(PIN_MUX_SEL, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_GPIOA0_1));
		#else
		RTL_W32(PIN_MUX_SEL, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_POCKETAP_DATA));
		#endif
		//set gpioA[2~6] as GPIO PIN
		RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & ~(RTL_GPIO_CNR_POCKETAP_DATA)));	
		//set direction, GPIOA[3,4,5] INPUT, GPIOA[2,6] OUTPUT
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA3))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA4))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA5))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOA2))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOA6))));
	#elif defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8196E)
		//reg_iocfg_jtag config as gpio mode,gpioA[2~6]
		#ifndef CONFIG_USING_JTAG
		RTL_W32(PIN_MUX_SEL, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_POCKETAP_DATA));
		#endif
#if defined(CONFIG_RTL_96E_GPIOB5_RESET)
                RTL_W32(PIN_MUX_SEL_2, (RTL_R32(PIN_MUX_SEL_2) | (3<<9)));
#endif
		//set gpioA[2,4,5,6]as GPIO PIN
		RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & ~(RTL_GPIO_CNR_POCKETAP_DATA)));	
#if defined(CONFIG_RTL_96E_GPIOB5_RESET)
                RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & ~(1<<RESET_BTN_PIN)));
#endif
		//set direction, GPIOA[2,4,5] INPUT, GPIOA[6] OUTPUT
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA2))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA4))));
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA5))));
#if defined(CONFIG_RTL_96E_GPIOB5_RESET)
                RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(1<<RESET_BTN_PIN))));
#endif


	#if defined(CONFIG_RTL_ULINKER)
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA6))));
	#else
		RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOA6))));
	#endif

	#endif
	res = create_proc_entry("gpio", 0, NULL);
	if (res) {
		res->read_proc = read_proc;
		res->write_proc = write_proc;
	}
	else {
		printk("Realtek GPIO Driver, create proc failed!\n");
	}

	res = create_proc_entry("usb_mode_detect", 0, NULL);
	if (res) {
		res->read_proc = usb_mode_detect_read_proc;
		res->write_proc = NULL;
	}
	else
	{
		printk("<%s>LZQ: after create_proc_entry, res is NULL\n", __FUNCTION__);
	}

        #ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
        res = create_proc_entry("bootbank", 0, NULL);
        if (res) {
                res->read_proc = read_bootbank_proc;
                //res->write_proc = write_bootbank_proc;
        }
        else {
                printk("read bootbank, create proc failed!\n");
        }
        #endif
	//return;
#else //defined CONFIG_RTL_819XD

	// Set GPIOA pin 10(8181)/0(8186) as input pin for reset button

#if  defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198)
#ifndef CONFIG_USING_JTAG
	RTL_W32(RTL_GPIO_MUX, (RTL_R32(RTL_GPIO_MUX) | (RTL_GPIO_MUX_DATA)));
#endif	
#if defined(CONFIG_RTL_8198)
	RTL_W32(RTL_GPIO_MUX, (RTL_R32(RTL_GPIO_MUX) | 0xf));
#endif
#if defined(CONFIG_RTL_8196CS)
extern int PCIE_reset_procedure(int PCIE_Port0and1_8196B_208pin, int Use_External_PCIE_CLK, int mdio_reset,unsigned long conf_addr);
#define CLK_MANAGE	0xb8000010
#define PCI_CONFIG_COMMAND		(0xB8B10000+4)
#define PCI_CONFIG_LATENCY			(0xB8B10000+0x0c)
#define PCI_CONFIG_BASE0			(0xB8B10000+0x10)
#define PCI_CONFIG_BASE1			(0xB8B10000+0x18)
	extern void setBaseAddressRegister(void);
		RTL_W32(CLK_MANAGE, (RTL_R32(CLK_MANAGE) | BIT(11)));
		mdelay(10);
		PCIE_reset_procedure(0, 0, 1, 0xb8b10000);
		setBaseAddressRegister();	
		{

			int i=0;
			*((volatile unsigned long *)PCI_CONFIG_BASE1) = 0x19000000;
			//DEBUG_INFO("...config_base1 = 0x%08lx\n", *((volatile unsigned long *)PCI_CONFIG_BASE1));
			for(i=0; i<1000000; i++);
			*((volatile unsigned char *)PCI_CONFIG_COMMAND) = 0x07;
			//DEBUG_INFO("...command = 0x%08lx\n", *((volatile unsigned long *)PCI_CONFIG_COMMAND));
			for(i=0; i<1000000; i++);
			*((volatile unsigned short *)PCI_CONFIG_LATENCY) = 0x2000;
			for(i=0; i<1000000; i++);
			//DEBUG_INFO("...latency = 0x%08lx\n", *((volatile unsigned long *)PCI_CONFIG_LATENCY));
		}
		
#endif
#ifdef CONFIG_POCKET_ROUTER_SUPPORT

//panic_printk("\r\n 0x%x__[%s-%u]",RTL_R32(RTL_GPIO_MUX),__FILE__,__LINE__);	
#ifndef CONFIG_USING_JTAG
	RTL_W32(RTL_GPIO_MUX, (RTL_R32(RTL_GPIO_MUX) | (RTL_GPIO_MUX_POCKETAP_DATA)));
#endif	
//panic_printk("\r\n 0x%x__[%s-%u]",RTL_R32(RTL_GPIO_MUX),__FILE__,__LINE__);	
	RTL_W32(PABCD_CNR, (RTL_R32(PABCD_CNR) & (~(RTL_GPIO_CNR_POCKETAP_DATA))));





	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOA2))));

	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOB3))));

	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) | ((RTL_GPIO_DIR_GPIOB2))));

	RTL_W32(PABCD_DIR, (RTL_R32(PABCD_DIR) & (~(RTL_GPIO_DIR_GPIOC0))));	

#endif //end of CONFIG_POCKET_ROUTER_SUPPORT 
#if defined(CONFIG_RTL_8196CS)
	RTL_W32(PCIE_PIN_MUX,(RTL_R32(PCIE_PIN_MUX)&(~(1<<29)))|(1<<13) );
	RTL_W32(RESET_PIN_IOBASE, (RTL_R32(RESET_PIN_IOBASE) & (~((1 << RESET_BTN_PIN)<<MODE_MARSK))));
	RTL_W32(RESET_PIN_DIRBASE, (RTL_R32(RESET_PIN_DIRBASE) & (~((1 << RESET_BTN_PIN)<<DIR_MASK  ))));

#else
	RTL_W32(RESET_PIN_IOBASE, (RTL_R32(RESET_PIN_IOBASE) & (~(1 << RESET_BTN_PIN))));
	RTL_W32(RESET_PIN_DIRBASE, (RTL_R32(RESET_PIN_DIRBASE) & (~(1 << RESET_BTN_PIN))));
#endif
#if defined(READ_RF_SWITCH_GPIO)
	RTL_W32(WIFI_ONOFF_PIN_IOBASE, (RTL_R32(WIFI_ONOFF_PIN_DIRBASE) & ( ~(1<<RTL_GPIO_WIFI_ONOFF))));
	RTL_W32(WIFI_ONOFF_PIN_DIRBASE, (RTL_R32(WIFI_ONOFF_PIN_DIRBASE) & (~(1<<RTL_GPIO_WIFI_ONOFF))));
	RTL_W32(WIFI_ONOFF_PIN_DATABASE, (RTL_R32(WIFI_ONOFF_PIN_DATABASE) & (~(1<<RTL_GPIO_WIFI_ONOFF))));

#endif // #if defined(READ_RF_SWITCH_GPIO)
#endif // #if defined(CONFIG_RTL865X)
#ifdef RESET_LED_NO
#if  defined(CONFIG_RTL_8196CS)
	RTL_W32(RESET_LED_IOBASE, (RTL_R32(RESET_LED_IOBASE) | (((1 << RESET_LED_PIN)))));
	RTL_W32(RESET_LED_DIRBASE, (RTL_R32(RESET_LED_DIRBASE) | ((1 << RESET_LED_PIN))));
#else
	// Set GPIOA ping 2 as output pin for reset led
	RTL_W32(RESET_LED_DIRBASE, (RTL_R32(RESET_LED_DIRBASE) | ((1 << RESET_LED_PIN))));
#endif
#endif
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	
	res = create_proc_entry("bootbank", 0, NULL);
	if (res) {
		res->read_proc = read_bootbank_proc;
		//res->write_proc = write_bootbank_proc;
	}
	else {
		printk("read bootbank, create proc failed!\n");
	}
#endif
#ifdef AUTO_CONFIG
	res = create_proc_entry("gpio", 0, NULL);
	if (res) {
		res->read_proc = read_proc;
		res->write_proc = write_proc;
	}
	else {
		printk("Realtek GPIO Driver, create proc failed!\n");
	}


#ifdef	USE_INTERRUPT_GPIO
#ifdef  IMMEDIATE_PBC
	res = create_proc_entry("gpio_wscd_pid", 0, NULL);
	if (res)
	{
		res->read_proc = read_gpio_wscd_pid;
		res->write_proc = write_gpio_wscd_pid;
		DPRINTK("create gpio_wscd_pid OK!!!\n\n");
	}
	else{
		printk("create gpio_wscd_pid failed!\n\n");
	}
#endif	
#endif


// 2009-0414		
#if defined(USE_INTERRUPT_GPIO)
#if defined(CONFIG_RTL_8198) && !defined(CONFIG_RTK_VOIP)
	RTL_R32(AUTOCFG_PIN_IMR) |= (0x01 << (AUTOCFG_BTN_PIN-16)*2); // enable interrupt in falling-edge		
#else
	RTL_R32(AUTOCFG_PIN_IMR) |= (0x01 << AUTOCFG_BTN_PIN*2); // enable interrupt in falling-edge	
#endif
	if (request_irq(BSP_GPIO_ABCD_IRQ, gpio_interrupt_isr, IRQF_SHARED, "rtl_gpio", (void *)&priv_gpio_wps_device)) {
		printk("gpio request_irq(%d) error!\n", GPIO_IRQ_NUM);		
   	}
#endif
// 2009-0414		
#endif

#endif//defined CONFIG_RTL_819XD

	res = create_proc_entry("load_default", 0, NULL);
	if (res) {
		res->read_proc = default_read_proc;
		res->write_proc = default_write_proc;
	}

#ifdef READ_RF_SWITCH_GPIO
	res = create_proc_entry("rf_switch", 0, NULL);
	if (res) {
		res->read_proc = rf_switch_read_proc;
		res->write_proc = NULL;
	}
#endif



#ifdef CONFIG_POCKET_ROUTER_SUPPORT

	res = create_proc_entry("dc_pwr", 0, NULL);
	if (res)
		res->read_proc = read_dc_pwr_proc;
	else
		printk("create read_dc_pwr_proc failed!\n");

	res = create_proc_entry("dc_pwr_plugged_flag", 0, NULL);
	if (res)
	{
		res->read_proc = read_dc_pwr_plugged_flag_proc;
	}
	else
		printk("create read_pocketAP_hw_set_flag_proc failed!\n");
	
	res = create_proc_entry("ap_client_rou", 0, NULL);
	if (res)
		res->read_proc = read_ap_client_rou_proc;
	else
		printk("create read_ap_client_rou_proc failed!\n");

	res = create_proc_entry("pocketAP_hw_set_flag", 0, NULL);
	if (res)
	{
		res->read_proc = read_pocketAP_hw_set_flag_proc;
		res->write_proc = write_pocketAP_hw_set_flag_proc;
	}
	else
		printk("create read_pocketAP_hw_set_flag_proc failed!\n");

	res = create_proc_entry("phyif", 0, NULL);
	if (res)
		res->read_proc = read_EnablePHYIf_proc;
	else
		printk("create read_EnablePHYIf_proc failed!\n");
				
	init_timer(&pocket_ap_timer);
	pocket_ap_timer.data = (unsigned long)NULL;
	pocket_ap_timer.function = &pocket_ap_timer_func;
	mod_timer(&pocket_ap_timer, jiffies + HZ);
#endif


#ifdef CONFIG_EFM_PATCH
        init_iptime_led_spec();
#endif


#if defined(CONFIG_RTL_ULINKER)
	res = create_proc_entry("ulinker_ap_cl", 0, NULL);
	if (res)
	{
		res->read_proc = read_ulinker_ap_cl;
	}
	else
		printk("create ulinker_ap_cl_proc failed!\n");

	res = create_proc_entry("ulinker_ap_cl_switching", 0, NULL);
	if (res)
	{
		res->read_proc = read_ulinker_ap_cl_switching;
		res->write_proc = write_ulinker_ap_cl_switching;
	}
	else
		printk("create ulinker_ap_cl_proc failed!\n");

#if defined(CONFIG_RTL_ULINKER_WLAN_DELAY_INIT)
	res_wlan=create_proc_entry("wlan_init",0,NULL);
	if (res_wlan) {
		res_wlan->write_proc=wlan_init_proc_write;
	}
	else
		printk("create wlan_init failed!\n");
#endif

#if 1//OTG_MODE_SWITCH
	res = create_proc_entry("otg_gadget", 0, NULL);
	if (res)
	{
		res->read_proc  = otg_proc_read_gadget;
		res->write_proc = otg_proc_write_gadget;
	}
	else
		printk("create otg_gadget failed!\n");

	res = create_proc_entry("wall_mount", 0, NULL);
	if (res)
	{
		res->read_proc  = otg_proc_read_wall_mount;
	}
	else
		printk("create wall_mount failed!\n");

	res = create_proc_entry("ulinker_cdc_rndis_status", 0, NULL);
	if (res)
	{
		res->read_proc  = otg_proc_read_cdc_rndis_status;
	}
	else
		printk("create ulinker_cdc_rndis_status failed!\n");

	res = create_proc_entry("ether_state", 0, NULL);
	if (res)
	{
		res->read_proc	= otg_proc_read_ether_state;
	}
	else
		printk("create ether_state failed!\n");

	res = create_proc_entry("fsg_state", 0, NULL);
	if (res)
	{
		res->read_proc	= otg_proc_read_fsg_state;
	}
	else
		printk("create fsg_state failed!\n");
#endif

	/* otg eth mac */
	res = create_proc_entry("ulinker_rndis_mac", 0, NULL);
	if (res)
	{
		res->read_proc  = read_ulinker_rndis_mac;
		res->write_proc = write_ulinker_rndis_mac;
	}
	else
		printk("create ulinker_rndis_mac failed!\n");

#if 0//defined(CONFIG_RTL_ULINKER_BRSC)
	res = create_proc_entry("brsc", 0, NULL);
	if (res)
	{
		res->read_proc	= read_ulinker_brsc_en;
		res->write_proc = write_ulinker_brsc_en;
	}
	else
		printk("create brsc failed!\n");
#endif

#if defined(CONFIG_RTL_ULINKER)
		res = create_proc_entry("uled", 0, NULL);
		if (res)
		{
			res->write_proc = write_ulinker_led;			
		}
		else
			printk("create uled failed!\n");
#endif

	#if 0 /* dip switch has been removed */
	init_timer(&ulinker_timer);
	ulinker_timer.data = (unsigned long)NULL;
	ulinker_timer.function = &ulinker_timer_func;
	mod_timer(&ulinker_timer, jiffies + HZ);
	#endif
#endif


	res = create_proc_entry("watchdog_reboot", 0, NULL);
	if (res) {
		res->write_proc = write_watchdog_reboot;
	}
	else
		printk("create watchdog_reboot failed!\n");
			
	init_timer(&probe_timer);
	probe_counter = 0;
	probe_state = PROBE_NULL;
	probe_timer.expires = jiffies + HZ;
	probe_timer.data = (unsigned long)NULL;
	probe_timer.function = &rtl_gpio_timer;
	mod_timer(&probe_timer, jiffies + HZ);


#ifdef CONFIG_EFM_PATCH
#ifdef WPS_BT_GPIO_BIT
	init_timer(&wps_timer);
	wps_timer.expires = jiffies + HZ;
	wps_timer.data = (unsigned long)NULL;
	wps_timer.function = &rtl_wps_timer;
	mod_timer(&wps_timer, jiffies + HZ/10);
#endif
#endif

	autoconfig_gpio_init(); //always init wps gpio
#ifdef CONFIG_RTL865X_CMO
	extra_led_gpio_init();
#endif
	return 0;
}


static void __exit rtl_gpio_exit(void)
{
	printk("Unload Realtek GPIO Driver \n");
	del_timer_sync(&probe_timer);
}


module_exit(rtl_gpio_exit);
module_init(rtl_gpio_init);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO driver for Reload default");
