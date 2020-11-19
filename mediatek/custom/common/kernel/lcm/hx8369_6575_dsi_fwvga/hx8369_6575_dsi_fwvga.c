/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
//#include <linux/string.h>
/*delete here*/
#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#else
#include <mach/mt_gpio.h>
#include <linux/string.h>
#endif
#include <cust_gpio_usage.h>


#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(854)
#define LCM_ID       (0x69)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER
#define LCD_MODUL_ID (0x01)
#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#define LCM_DSI_CMD_MODE									1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    

struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table hx8369_lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/
	/*modify the LCD parameter*/
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 5, {}},
        {0xB1,	19,	{0x01, 0x00, 0x34,0x04,
                                      0x00,0x0f,0x0f,0x25,
                                      0x2d,0x3f,0x3f,0x01,
                                      0x12,0x01,0xe6,0xe6,
                                      0xe6,0xe6,0xe6}},
        {REGFLAG_DELAY, 10, {}},
         {0xB2,	15,	{0x00,0x10,0x0A,0x0A,
                                      0x70,0x00,0xFF,0x00,
                                      0x00,0x01,0x90,0x03,  //when open tearing ,change 0x00,0x00 ->0x01,0x90 
                                      0x03,0x00,0x01}},
         {REGFLAG_DELAY, 5, {}},
         {0xB4,	5,	{0x00,0x18,0x78,0x06,0x02}},
         {REGFLAG_DELAY, 5, {}},
         {0x36,	1,	{0xD0}},
         {REGFLAG_DELAY, 5, {}},
//         {0xCC,	1,	{0x02}},
//         {REGFLAG_DELAY, 5, {}},
         /*delete set vcom reg*/
         //{0xB6,	2,	{0x3D,0x3D}},
         //{REGFLAG_DELAY, 5, {}},
         {0xB0,	2,	{0x01,0x09}},   //09 58hz  0b 68hz  0d 78hZ
         // ENABLE FMARK
         {0x35,	1,	{0x00}},
         {0x44,	2,	{0x01,0x90}},  //close tearing ,when open tearing,B2h byte9 byte10 is 0x01,0x90
         
         {0xD5,	26,	{0x00,0x01,0x03,0x28,
                                     0x01,0x09,0x00,0x70,
                                     0x11,0x13,0x00,0x00,
                                     0x60,0x24,0x71,0x35,
                                     0x00,0x00,0x71,0x05,
                                     0x60,0x24,0x07,0x0F,
                                     0x04,0x04}},
         {REGFLAG_DELAY, 10, {}},
         //Gamma2.5 setting
	 {0xe0,      34,    {0x00,0x14,0x1D,0x3B,
	                             0x3F,0x3F,0x30,0x4C,
	                             0x06,0x0C,0x0F,0x13,
	                             0x15,0x13,0x14,0x12,
	                             0x14,0x00,0x14,0x1D,
	                             0x3B,0x3F,0x3F,0x30,
	                             0x4C,0x06,0x0C,0x0F,
	                             0x13,0x15,0x13,0x14,
	                             0x12,0x14} },
         {REGFLAG_DELAY, 10, {}},
	{0x3A,      1,      {0x77}},
	/* for mipi */
	{0xBA,      13,    {0x00,0xA0,0xC6,0x80,
	                             0x0A,0x00,0x10,0x30,
	                             0x6F,0x02,0x11,0x18,
	                             0x40}},	
	 /* SET DISPLAY ON */	
	{0x11,      0,    {}},
	{REGFLAG_DELAY, 120, {}},
	{0x29,      0,    {}},
	{REGFLAG_DELAY, 20, {}},
//         {0x2C,      0,    {}},
//         {REGFLAG_DELAY, 100, {}},
	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.


	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#if 0
static void hx8369_fwvga_init_setting(void)
{
    unsigned int data_array[16];
	
    //{0xB9,	3,	{0xFF, 0x83, 0x69}}
    data_array[0]=0x00043902;
    data_array[1]=0x6983FFB9;
    dsi_set_cmdq(&data_array,2,1);

    MDELAY(5);

    /*{0xB1,	19,	{0x01, 0x00, 0x34,0x04,
                                      0x00,0x0f,0x0f,0x25,
                                      0x2d,0x3f,0x3f,0x01,
                                      0x12,0x01,0xe6,0xe6,
                                      0xe6,0xe6,0xe6}},
    */
    data_array[0]=0x00143902;
    data_array[1]=0x340001B1;
    data_array[2]=0x0F0F0004;
    data_array[3]=0x3F3F2D25;
    data_array[4]=0xE6011201;
    data_array[5]=0xE6E6E6E6;
    dsi_set_cmdq(&data_array,6,1);

    MDELAY(10);

    /*{0xB2,	15,	{0x00,0x10,0x0A,0x0A,
                                      0x70,0x00,0xFF,0x00,
                                      0x00,0x01,0x90,0x03,  //when open tearing ,change 0x00,0x00 ->0x01,0x90 
                                      0x03,0x00,0x01}},
    */
    data_array[0]=0x00103902;
    data_array[1]=0x0A1000B2;
    data_array[2]=0xFF00700A;
    data_array[3]=0x90010000;
    data_array[4]=0x01000303;
    dsi_set_cmdq(&data_array,5,1);

    MDELAY(5);

    /*{0xB4,	5,	{0x00,0x18,0x78,0x06,0x02}}*/
    data_array[0]=0x00063902;
    data_array[1]=0x781800B4;
    data_array[2]=0x00000206;
    dsi_set_cmdq(&data_array,3,1);

    MDELAY(5);

    /*{0x36,	1,	{0xD0}}*/
    data_array[0]=0xD0361500;
    dsi_set_cmdq(&data_array,1,1);

    MDELAY(5);

    /*{0xB6,	2,	{0x3D,0x3D}}*/
    /* haimou tell me delete
    data_array[0]=0x00033902;
    data_array[1]=0x003D3DB6;
    dsi_set_cmdq(&data_array,2,1);

    MDELAY(5);
    */
    //Increse rate is 59Hz
    /*{0xB0,	2,	{0x01,0x09}},   //09 55hz  0a 59hz  0b 62hz  0d 75hZ*/
    data_array[0]=0x00033902;
    data_array[1]=0x000A01B0; 
    dsi_set_cmdq(&data_array,2,1);

    /*{0x35,	1,	{0x00}}*/
    data_array[0]=0x00351500;
    dsi_set_cmdq(&data_array,1,1);

    /*{0x44,	2,	{0x01,0x90}}  close tearing ,when open tearing,B2h byte9 byte10 is 0x01,0x90*/
    data_array[0]=0x00033902;
    data_array[1]=0x00900144;
    dsi_set_cmdq(&data_array,2,1);

    /*{0xD5,	26,	{0x00,0x01,0x03,0x28,
                                     0x01,0x09,0x00,0x70,
                                     0x11,0x13,0x00,0x00,
                                     0x60,0x24,0x71,0x35,
                                     0x00,0x00,0x71,0x05,
                                     0x60,0x24,0x07,0x0F,
                                    0x04,0x04}},
     */
    data_array[0]=0x001B3902;
    data_array[1]=0x030100D5;
    data_array[2]=0x00090128;
    data_array[3]=0x00131170;
    data_array[4]=0x71246000;
    data_array[5]=0x71000035;
    data_array[6]=0x07246005;
    data_array[7]=0x0004040F;
    dsi_set_cmdq(&data_array,8,1);

    MDELAY(10);

    /*
    {0xe0,      34,    {0x00,0x18,0x21,0x3B,
	                             0x3F,0x3F,0x36,0x4B,
	                             0x0A,0x0F,0x0F,0x13,
	                             0x15,0x13,0x14,0x14,
	                             0x14,0x00,0x18,0x21,
	                             0x3B,0x3F,0x3F,0x31,
	                             0x50,0x0A,0x0F,0x0F,
	                             0x13,0x15,0x13,0x14,
	                             0x14,0x14} },
    */
    data_array[0]=0x00233902;
    data_array[1]=0x211800E0;
    data_array[2]=0x363F3F3B;
    data_array[3]=0x0F0F0A4B;
    data_array[4]=0x14131513;
    data_array[5]=0x18001414;
    data_array[6]=0x3F3F3B21;
    data_array[7]=0x0F0A5031;
    data_array[8]=0x1315130F;
    data_array[9]=0x00141414;
    dsi_set_cmdq(&data_array,10,1);

    MDELAY(10);

    /*{0x3A,      1,      {0x77}}*/
    data_array[0] = 0x773A1500;
    dsi_set_cmdq(&data_array,1,1);

    /*{0xBA,      13,    {0x00,0xA0,0xC6,0x00,
	                             0x0A,0x00,0x10,0x30,
	                             0x6F,0x02,0x11,0x18,
	                             0x40}},	
    */
    data_array[0]=0x000E3902;
    data_array[1]=0xC6A000BA;
    data_array[2]=0x10000A80;
    data_array[3]=0x11026F30;
    data_array[4]=0x00004018;
    dsi_set_cmdq(&data_array,5,1);

    /* SET DISPLAY ON	
	{0x11,      0,    {}},
    */
    data_array[0]=0x00110500;
    dsi_set_cmdq(&data_array,1,1);

    MDELAY(120);

    /*{0x29,      0,    {}},*/
    data_array[0]=0x00290500;
    dsi_set_cmdq(&data_array,1,1);
    MDELAY(20);
    
}
#endif
#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 0, {}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 0, {}},
	{REGFLAG_DELAY, 20, {}},

    // Sleep Mode On
	{0x10, 0, {}},
	{REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
	// Display off sequence
	{0xB9,	3,	{0xFF, 0x83, 0x69}},
	{REGFLAG_DELAY, 10, {}},

    // Sleep Mode On
//	{0xC3, 1, {0xFF}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{0x53, 1, {0x24}},
	{0x55, 1, {0x00}}, //close cabc
	{0xC9, 7, {0x3e,0x00,0x00,0x01,0x0F,0x02,0x1e}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

//delete mipi timing , use default timing , open TE function
static void lcm_get_params(LCM_PARAMS *params)
{
        memset(params, 0, sizeof(LCM_PARAMS));

        params->type   = LCM_TYPE_DSI;

        params->width  = FRAME_WIDTH;
        params->height = FRAME_HEIGHT;

        // enable tearing-free
        params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
        params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
        //params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
#if (LCM_DSI_CMD_MODE)
        params->dsi.mode   = CMD_MODE;
#else
        params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif

        // DSI
        /* Command mode setting */
        params->dsi.LANE_NUM				= LCM_TWO_LANE;
        //The following defined the fomat for data coming from LCD engine.
        params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
        params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
        params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
        params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

        // Highly depends on LCD driver capability.
        // Not support in MT6573
        params->dsi.packet_size=256;

        // Video mode setting		
        params->dsi.intermediat_buffer_num = 2;

        params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
        params->dsi.word_count=480*3;	

        params->dsi.vertical_sync_active=3;
        params->dsi.vertical_backporch=12;
        params->dsi.vertical_frontporch=2;

        params->dsi.vertical_active_line = FRAME_HEIGHT;

        params->dsi.horizontal_sync_active = 10;
        params->dsi.horizontal_backporch = 50;
        params->dsi.horizontal_frontporch = 50;
        params->dsi.horizontal_active_pixel = FRAME_WIDTH;

        //227 MHz
        params->dsi.pll_div1=1;/*b00214920 test 0--> 1*/
        params->dsi.pll_div2=1;
        params->dsi.fbk_div=34;        
}
//rst pin is high->low->high
static void lcm_init(void)
{
    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);  //huawei use GPIO 49: LSA0 to be reset pin
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(5);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(10);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);

    #ifdef BUILD_LK
        printf("uboot:hx8369_lcm_init\n");
    #else
        printk("kernel:hx8369_lcm_init\n");
    #endif
    //hx8369_fwvga_init_setting();
    push_table(hx8369_lcm_initialization_setting, sizeof(hx8369_lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
    #ifdef BUILD_LK
        printf("uboot:hx8369_lcm_suspend\n");
    #else
        printk("kernel:hx8369_lcm_suspend\n");
    #endif
    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}

//when phone resume , intialize lcd again;
static void lcm_resume(void)
{
	lcm_init();
	#ifdef BUILD_LK
		 printf("uboot:hx8369_lcm_resume\n");
	#else
		 printk("kernel:hx8369_lcm_resume\n");
	#endif
}

static void lcm_update(unsigned int x, unsigned int y,
unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

    unsigned char x0_MSB = ((x0>>8)&0xFF);
    unsigned char x0_LSB = (x0&0xFF);
    unsigned char x1_MSB = ((x1>>8)&0xFF);
    unsigned char x1_LSB = (x1&0xFF);
    unsigned char y0_MSB = ((y0>>8)&0xFF);
    unsigned char y0_LSB = (y0&0xFF);
    unsigned char y1_MSB = ((y1>>8)&0xFF);
    unsigned char y1_LSB = (y1&0xFF);

    unsigned int data_array[16];
/* shorten the update time to improve LCD update speed */
    data_array[0]= 0x00053902;
    data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
    data_array[2]= (x1_LSB);
    dsi_set_cmdq(&data_array, 3, 1);

    data_array[0]= 0x00053902;
    data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
    data_array[2]= (y1_LSB);
    dsi_set_cmdq(&data_array, 3, 1);

    data_array[0]= 0x00290508;
    dsi_set_cmdq(&data_array, 1, 1);

    data_array[0]= 0x002c3909;
    dsi_set_cmdq(&data_array, 1, 0);

}

//reduce lcd backlight , decrease power consume
static void lcm_setbacklight(unsigned int level)
{
        unsigned int default_level = 145;
        unsigned int mapped_level = 0;
        unsigned int data_array[2];
        unsigned int backlight = level & 0xFF ;    
        //reduce lcd backlight
        backlight = backlight * 8/10;
        data_array[0] = 0x00511500 | backlight << 24; 
        dsi_set_cmdq(&data_array, 1, 1);
#ifdef BUILD_LK
        printf("uboot:hx8369_lcm_setbacklight backlight = %d,level=%d\n",backlight,level);
#else
        printk("kernel:hx8369_lcm_setbacklight backlight = %d,level=%d\n",backlight,level);
#endif
        }

static unsigned int lcm_esd_check(void)
{
		int value = 0;
#ifndef BUILD_LK
        if(lcm_esd_test)
        {
            lcm_esd_test = FALSE;
            return TRUE;
        }

        /// please notice: the max return packet size is 1
        /// if you want to change it, you can refer to the following marked code
        /// but read_reg currently only support read no more than 4 bytes....
        /// if you need to read more, please let BinHan knows.
        /*
                unsigned int data_array[16];
                unsigned int max_return_size = 1;
                
                data_array[0]= 0x00003700 | (max_return_size << 16);    
                
                dsi_set_cmdq(&data_array, 1, 1);
        */
        value = read_reg(0xB6);        
        #ifdef BUILD_LK
            printf("uboot:hx8369_lcm_esd_check read_reg 0xB6 = %d\n",value);
        #else
            printk("kernel:hx8369_lcm_esd_check read_reg 0xB6 = %d\n",value);
        #endif
        if(value == 0x42)
        {
            return FALSE;
        }
        else
        {            
            return TRUE;
        }
#endif
}

static unsigned int lcm_esd_recover(void)
{
    unsigned char para = 0;

    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);
    SET_RESET_PIN(1);
    MDELAY(120);
    #ifdef BUILD_LK
        printf("uboot:hx8369_lcm_esd_recover\n");
    #else
        printk("kernel:hx8369_lcm_esd_recover\n");
    #endif
	  push_table(hx8369_lcm_initialization_setting, sizeof(hx8369_lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
	  push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);
    dsi_set_cmdq_V2(0x35, 1, &para, 1);     ///enable TE
    MDELAY(10);

    return TRUE;
}

static unsigned int lcm_compare_id(void)
{
#if 0
	unsigned int id = 0;
	unsigned char buffer[2];
	unsigned int array[16];
        lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);  //huawei use GPIO 49: LSA0 to be reset pin
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(25);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(50);
        #ifdef BUILD_LK
            printf("uboot:hx8369_lcm_compare_id\n");
        #else
            printk("kernel:hx8369_lcm_compare_id\n");
        #endif
	push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);

	array[0] = 0x00023700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
//	id = read_reg(0xF4);
	read_reg_v2(0xF4, buffer, 2);
	id = buffer[0]; //we only need ID
#if defined(BUILD_LK)
	printf("%s, id1 = 0x%08x\n", __func__, id);
#endif
    return (LCM_ID == id)?1:0;
#else
/*delete here*/
    return ((which_lcd_module() == LCD_MODUL_ID) ? 1:0);
#endif
}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hx8369_dsi_6575_fwvga_lcm_drv = 
{
    .name			= "hx8369_dsi_6575_fwvga",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
	.set_backlight	= lcm_setbacklight,
//	.set_pwm        = lcm_setpwm,
//	.get_pwm        = lcm_getpwm,
	//.esd_check   = lcm_esd_check,
         //.esd_recover   = lcm_esd_recover,
	.compare_id    = lcm_compare_id,
#endif
};
