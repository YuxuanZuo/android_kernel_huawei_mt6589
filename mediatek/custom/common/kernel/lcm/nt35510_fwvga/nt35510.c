/* Copyright Statement:
*
* This software/firmware and related documentation ("MediaTek Software") are
* protected under relevant copyright laws. The information contained herein
* is confidential and proprietary to MediaTek Inc. and/or its licensors.
* Without the prior written permission of MediaTek inc. and/or its licensors,
* any reproduction, modification, use or disclosure of MediaTek Software,
* and information contained herein, in whole or in part, shall be strictly prohibited.
*/
/* MediaTek Inc. (C) 2010. All rights reserved.
*
* BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
* THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
* CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
* SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
* STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
* CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* The following software/firmware and/or related documentation ("MediaTek Software")
* have been modified by MediaTek Inc. All revisions are subject to any receiver's
* applicable license agreements with MediaTek Inc.
*/

/*****************************************************************************
* Copyright Statement:
* --------------------
* This software is protected by Copyright and the information contained
* herein is confidential. The software may not be copied and the information
* contained herein may not be used or disclosed except with the written
* permission of MediaTek Inc. (C) 2008
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
* THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
* RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
* AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
* NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
* SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
* SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
* THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
* NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
* SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
* AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
* OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
* MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
* LAWS PRINCIPLES. ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
* RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
* THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

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
// Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH (480)
#define FRAME_HEIGHT (854)

#define LCM_ID_NT35510 (0x5510)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER
#define LCD_MODUL_ID (0x00)
// ---------------------------------------------------------------------------
// Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

/*************/
#define LSDA_GPIO_PIN (GPIO_DISP_LSDA_PIN)

#define SET_GPIO_OUT(n, v) (lcm_util.set_gpio_out((n), (v)))

#define SET_LSDA_LOW SET_GPIO_OUT(LSDA_GPIO_PIN, 0)
#define SET_LSDA_HIGH SET_GPIO_OUT(LSDA_GPIO_PIN, 1)

/***************/


#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
// Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
//#define read_reg lcm_util.dsi_read_reg()
#define read_reg(cmd) lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)


#define LCM_DSI_CMD_MODE


struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

/*modify the LCD parameter*/
static struct LCM_setting_table nt35510_lcm_initialization_setting[] = {

    {0xf0,  5,  {0x55,0xaa,0x52,0x08,0x01}},

    {0xb6,  3,  {0x34,0x34,0x34}},    //Power control AVDD
    {0xb0,  3,  {0x09,0x09,0x09}},    //AVDD Voltage 6.5-(12X0.1)=5.3
    {0xb7,  3,  {0x24,0x24,0x24}},    //Power control AVDD Booster time and booster frequency -2.0*VDDB/(HS/2)

    {0xb1,  3,  {0x09,0x09,0x09}},    //AVEE Voltage 6.5-(12X0.1)=5.3
    {0xb8,  3,  {0x34,0x34,0x34}},    //Power control VCL -2*VDDB,HS/2
    {0xb2,  3,  {0x01,0x01,0x01}},    //VCL Voltage -2.5V
    {0xb9,  3,  {0x24,0x24,0x24}},    //Power control VGH AVDD-AVEE+VDDB,HS/2
    {0xb3,  3,  {0x05,0x05,0x05}},    //VGH Voltage 12V
    {0xba,  3,  {0x34,0x34,0x34}},    //Power control VGLX AVEE+VCL-AVDD,HS/2
    {0xb5,  3,  {0x0b,0x0b,0x0b}},    //VGL_REG Voltage -12V

    {0xbc,  3,  {0x00,0xa3,0x00}},    //VGMP=3.0+128*0.0125=4.6V,VGSP=0V
    {0xbd,  3,  {0x00,0xa3,0x00}},    //VGMN=-(3.0+128*0.0125)=-4.6V,VGSN=0V
    //{0xbe,  2,  {0x00,0x58}},       //VCOM OFFSET Voltage -47*0.0125=-0.5875
    /*Solve the bug of BOE LCD abnormal noise when BOE LCD bright screen*/
    //{0xbf,  1,  {0x01}},              //VGH Output Voltage 01/7~18
    {0xd1,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7B,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},

    {0xd2,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},

    {0xd3,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},

    {0xd4,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},

    {0xd5,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},

    {0xd6,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},


    {0xf0,  5,  {0x55,0xaa,0x52,0x08,0x00}},    //
    {0xb5,  1,  {0x6b}},                        //Resolution set
    {0xb6,  1,  {0x0a}},                        //Source output data hold time 2.5us

    {0xb7,  2,  {0x00,0x00}},                   //EQ Control for gate signal 0us
    {0xb8,  4,  {0x01,0x05,0x05,0x05}},         //EQ Control for source signal 5*0.5=2.5us
    {0xba,  1,  {0x01}},                        //Source control in vertical porch time, off color
    {0xbc,  3,  {0x00,0x00,0x00}},              //Inversion driving control, column inversion
    {0xbd,  5,  {0x01,0x7A,0x07,0x31,0x00}},    //Display timing control:0x6A-60Hz   0x7A-58Hz
    {0xbe,  5,  {0x01,0x84,0x07,0x31,0x00}},
    {0xbf,  5,  {0x01,0x84,0x07,0x31,0x00}},

    {0xcc,  3,  {0x03,0x00,0x00}},              //Display timing control for VGSW[3:0]
    {0x3a,  1,  {0x77}},
    {0xb1,  2,  {0xEC,0x06}},                   //Display option control,TE/CRGB/Scanning
    //{0xb1,  2,  {0xF8,0x04}},                   //Display option control,TE/CRGB/Scanning
    {0x36,  1,  {0x46}},                        //MEMORY DATA ACCESS
    {0x35,  1,  {0x00}},                        //TE
    /*shorten the time of screen on*/
   {0x53, 1, {0x24}}, 
   {0x55, 1, {0x00}}, 
    {0x11,  0,  {}},                        //Sleep out
    {REGFLAG_DELAY, 120, {}},

    {0x29,  0,  {}},                        //Display on
    {REGFLAG_DELAY, 20, {}},

    //{0x2C,  0,  {}},                          //Memory write

    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

// ---------------------------------------------------------------------------
// LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}
static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if defined(LCM_DSI_CMD_MODE)
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

    params->dsi.packet_size=256;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    params->dsi.vertical_sync_active = 3;
    params->dsi.vertical_backporch = 12;
    params->dsi.vertical_frontporch = 2;
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
		params->dsi.horizontal_sync_active = 10;
    params->dsi.horizontal_backporch = 50;
    params->dsi.horizontal_frontporch = 50;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;

    params->dsi.HS_TRAIL = 10;
    //params->dsi.HS_TRAIL = 13;
    params->dsi.HS_ZERO = 13;
    //params->dsi.HS_ZERO = 10;
    params->dsi.HS_PRPR = 4;
    //params->dsi.HS_PRPR = 6;

    params->dsi.LPX = 13;   //26; // 13;
    params->dsi.TA_SACK = 1;
    params->dsi.TA_GET = 65;
    params->dsi.TA_SURE = 19;
    params->dsi.TA_GO = 5;

    params->dsi.CLK_TRAIL = 10;
    //params->dsi.CLK_TRAIL = 13;
    params->dsi.CLK_ZERO = 18; //13;

    params->dsi.LPX_WAIT = 10;

    params->dsi.CONT_DET = 0;

    params->dsi.CLK_HS_PRPR = 4;
  //params->dsi.CLK_HS_EXIT = 24;
    //params->dsi.DA_HS_EXIT = 8;
		params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4
		params->dsi.fbk_div =30;  //34; //20;    //34;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)

}

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
#if 0
void nt35510_init_setting(void)
{
	unsigned int data_array[16];
	
	//{0xf0,  5,  {0x55,0xaa,0x52,0x08,0x01}},
	data_array[0]=0x00063902;
	data_array[1]=0x52AA55F0;
	data_array[2]=0x00000108;
	dsi_set_cmdq(&data_array,3,1);

    //{0xb6,  3,  {0x34,0x34,0x34}},    //Power control AVDD
    //{0xb0,  3,  {0x09,0x09,0x09}},    //AVDD Voltage 6.5-(12X0.1)=5.3
    //{0xb7,  3,  {0x24,0x24,0x24}},    //Power control AVDD Booster time and booster frequency -2.0*VDDB/(HS/2)

	data_array[0]=0x00043902;
	data_array[1]=0x343434b6;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x090909b0;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x242424b7;
	dsi_set_cmdq(&data_array,2,1);

    //{0xb1,  3,  {0x09,0x09,0x09}},    //AVEE Voltage 6.5-(12X0.1)=5.3
    //{0xb8,  3,  {0x34,0x34,0x34}},    //Power control VCL -2*VDDB,HS/2
    //{0xb2,  3,  {0x01,0x01,0x01}},    //VCL Voltage -2.5V
    
	data_array[0]=0x00043902;
	data_array[1]=0x090909b1;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x343434b8;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x010101b2;
	dsi_set_cmdq(&data_array,2,1);

    //{0xb9,  3,  {0x24,0x24,0x24}},    //Power control VGH AVDD-AVEE+VDDB,HS/2
    //{0xb3,  3,  {0x05,0x05,0x05}},    //VGH Voltage 12V
    //{0xba,  3,  {0x34,0x34,0x34}},    //Power control VGLX AVEE+VCL-AVDD,HS/2
   
	data_array[0]=0x00043902;
	data_array[1]=0x242424b9;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x050505b3;
	dsi_set_cmdq(&data_array,2,1);

	data_array[0]=0x00043902;
	data_array[1]=0x343434ba;
	dsi_set_cmdq(&data_array,2,1); 
    
    //{0xb5,  3,  {0x0b,0x0b,0x0b}},    //VGL_REG Voltage -12V

    //{0xbc,  3,  {0x00,0xa3,0x00}},    //VGMP=3.0+128*0.0125=4.6V,VGSP=0V
    //{0xbd,  3,  {0x00,0xa3,0x00}},    //VGMN=-(3.0+128*0.0125)=-4.6V,VGSN=0V
    
data_array[0]=0x00043902;
data_array[1]=0x0b0b0bb5;
dsi_set_cmdq(&data_array,2,1);

data_array[0]=0x00043902;
data_array[1]=0x00a300bc;
dsi_set_cmdq(&data_array,2,1);

data_array[0]=0x00043902;
data_array[1]=0x00a300bd;
dsi_set_cmdq(&data_array,2,1);  

    //{0xbe,  2,  {0x00,0x58}},       //VCOM OFFSET Voltage -47*0.0125=-0.5875
    //{0xbf,  1,  {0x01}},              //VGH Output Voltage 01/7~18
    
data_array[0]=0x01bf1500;
dsi_set_cmdq(&data_array,1,1);


/*==============================
    {0xd1,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7B,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,                
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,                
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},
=================================*/
    data_array[0]=0x00353902;
    data_array[1]=0x000100D1;
    data_array[2]=0x00AF007F;
    data_array[3]=0x01E200CC;
    data_array[4]=0x0128010A;
    data_array[5]=0x017B0157;
    data_array[6]=0x02E301B7;
    data_array[7]=0x0265022C;
    data_array[8]=0x029B0267;
    data_array[9]=0x03F702D6;
    data_array[10]=0x03440325;
    data_array[11]=0x037B0365;
    data_array[12]=0x03B50397;
    data_array[13]=0x03FD03EA;
    data_array[14]=0x000000FE;
    dsi_set_cmdq(&data_array,15,1); 

/*==============================

    {0xd2,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},
=================================*/
    data_array[0]=0x00353902;
    data_array[1]=0x000100D2;
    data_array[2]=0x00AF007F;
    data_array[3]=0x01E200CC;
    data_array[4]=0x0128010A;
    data_array[5]=0x017B0157;
    data_array[6]=0x02E301B7;
    data_array[7]=0x0265022C;
    data_array[8]=0x029B0267;
    data_array[9]=0x03F702D6;
    data_array[10]=0x03440325;
    data_array[11]=0x037B0365;
    data_array[12]=0x03B50397;
    data_array[13]=0x03FD03EA;
    data_array[14]=0x000000FE;
    dsi_set_cmdq(&data_array,15,1);
/*==============================

    {0xd3,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},
=================================*/
    data_array[0]=0x00353902;
    data_array[1]=0x000100D3;
    data_array[2]=0x00AF007F;
    data_array[3]=0x01E200CC;
    data_array[4]=0x0128010A;
    data_array[5]=0x017B0157;
    data_array[6]=0x02E301B7;
    data_array[7]=0x0265022C;
    data_array[8]=0x029B0267;
    data_array[9]=0x03F702D6;
    data_array[10]=0x03440325;
    data_array[11]=0x037B0365;
    data_array[12]=0x03B50397;
    data_array[13]=0x03FD03EA;
    data_array[14]=0x000000FE;
    dsi_set_cmdq(&data_array,15,1);

/*==============================

    {0xd4,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},
=================================*/
    data_array[0]=0x00353902;
    data_array[1]=0x000100D4;
    data_array[2]=0x00AF007F;
    data_array[3]=0x01E200CC;
    data_array[4]=0x0128010A;
    data_array[5]=0x017B0157;
    data_array[6]=0x02E301B7;
    data_array[7]=0x0265022C;
    data_array[8]=0x029B0267;
    data_array[9]=0x03F702D6;
    data_array[10]=0x03440325;
    data_array[11]=0x037B0365;
    data_array[12]=0x03B50397;
    data_array[13]=0x03FD03EA;
    data_array[14]=0x000000FE;
    dsi_set_cmdq(&data_array,15,1);

/*==============================

    {0xd5,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},
=================================*/
    data_array[0]=0x00353902;
    data_array[1]=0x000100D5;
    data_array[2]=0x00AF007F;
    data_array[3]=0x01E200CC;
    data_array[4]=0x0128010A;
    data_array[5]=0x017B0157;
    data_array[6]=0x02E301B7;
    data_array[7]=0x0265022C;
    data_array[8]=0x029B0267;
    data_array[9]=0x03F702D6;
    data_array[10]=0x03440325;
    data_array[11]=0x037B0365;
    data_array[12]=0x03B50397;
    data_array[13]=0x03FD03EA;
    data_array[14]=0x000000FE;
    dsi_set_cmdq(&data_array,15,1);

/*==============================

    {0xd6,  52, {0x00,0x01,0x00,0x7f,
                 0x00,0xaf,0x00,0xcc,
                 0x00,0xe2,0x01,0x0a,
                 0x01,0x28,0x01,0x57,
                 0x01,0x7b,0x01,0xb7,
                 0x01,0xe3,0x02,0x2c,
                 0x02,0x65,0x02,0x67,
                 0x02,0x9b,0x02,0xd6,
                 0x02,0xf7,0x03,0x25,
                 0x03,0x44,0x03,0x65,
                 0x03,0x7b,0x03,0x97,
                 0x03,0xb5,0x03,0xea,
                 0x03,0xfd,0x03,0xfe}},
=================================*/
    data_array[0]=0x00353902;
    data_array[1]=0x000100D6;
    data_array[2]=0x00AF007F;
    data_array[3]=0x01E200CC;
    data_array[4]=0x0128010A;
    data_array[5]=0x017B0157;
    data_array[6]=0x02E301B7;
    data_array[7]=0x0265022C;
    data_array[8]=0x029B0267;
    data_array[9]=0x03F702D6;
    data_array[10]=0x03440325;
    data_array[11]=0x037B0365;
    data_array[12]=0x03B50397;
    data_array[13]=0x03FD03EA;
    data_array[14]=0x000000FE;
    dsi_set_cmdq(&data_array,15,1);


    //{0xf0,  5,  {0x55,0xaa,0x52,0x08,0x00}},    //
    //{0xb5,  1,  {0x6b}},                        //Resolution set
    //{0xb6,  1,  {0x0a}},                        //Source output data hold time 2.5us
	data_array[0]=0x00063902;
	data_array[1]=0x52AA55F0;
	data_array[2]=0x00000008;
	dsi_set_cmdq(&data_array,3,1);  
    
	data_array[0]=0x6BB51500;
	dsi_set_cmdq(&data_array,1,1);

	data_array[0]=0x0AB61500;
	dsi_set_cmdq(&data_array,1,1);

    //{0xb7,  2,  {0x00,0x00}},                   //EQ Control for gate signal 0us
    
    data_array[0]=0x00033902;
    data_array[1]=0x000000b7;
    dsi_set_cmdq(&data_array,2,1); 
    
    //{0xb8,  4,  {0x01,0x05,0x05,0x05}},         //EQ Control for source signal 5*0.5=2.5us
    data_array[0]=0x00053902;
    data_array[1]=0x050501B8;
    data_array[2]=0x00000005;
    dsi_set_cmdq(&data_array,3,1); 
    
    //{0xba,  1,  {0x01}},                        //Source control in vertical porch time, off color
    data_array[0]=0x01BA1500;
    dsi_set_cmdq(&data_array,1,1);
    
    //{0xbc,  3,  {0x00,0x00,0x00}},              //Inversion driving control, column inversion
    data_array[0]=0x00043902;
    data_array[1]=0x000000BC;
    dsi_set_cmdq(&data_array,2,1);
    
    //{0xbd,  5,  {0x01,0x6A,0x07,0x31,0x00}},    //Display timing control
    data_array[0]=0x00063902;
    data_array[1]=0x076A01BD;
    data_array[2]=0x00000031;
    dsi_set_cmdq(&data_array,3,1);
    
    //{0xbe,  5,  {0x01,0x84,0x07,0x31,0x00}},
    data_array[0]=0x00063902;
    data_array[1]=0x078401BE;
    data_array[2]=0x00000031;
    dsi_set_cmdq(&data_array,3,1); 
    
    //{0xbf,  5,  {0x01,0x84,0x07,0x31,0x00}},
    data_array[0]=0x00063902;
    data_array[1]=0x078401BF;
    data_array[2]=0x00000031;
    dsi_set_cmdq(&data_array,3,1);

    
    //{0xcc,  3,  {0x03,0x00,0x00}},              //Display timing control for VGSW[3:0]
    data_array[0]=0x00043902;
    data_array[1]=0x000003CC;
    dsi_set_cmdq(&data_array,2,1);
    
    //{0x3a,  1,  {0x77}},
    data_array[0]=0x773A1500;
    dsi_set_cmdq(&data_array,1,1);
    
    //{0xb1,  2,  {0xEC,0x06}},                   //Display option control,TE/CRGB/Scanning
    data_array[0]=0x00033902;
    data_array[1]=0x0006ECB1;
    dsi_set_cmdq(&data_array,2,1);
    
    //{0x36,  1,  {0x46}},                        //MEMORY DATA ACCESS
    data_array[0]=0x46361500;
    dsi_set_cmdq(&data_array,1,1);
    
    //{0x35,  1,  {0x00}},                        //TE
    data_array[0]=0x00351500;
    dsi_set_cmdq(&data_array,1,1);
    
    /*shorten the time of screen on*/
    //{0x53, 1, {0x24}}, 
    data_array[0]=0x24531500;
    dsi_set_cmdq(&data_array,1,1);
   
   
    //{0x55, 1, {0x00}}, 
    data_array[0]=0x00551500;
    dsi_set_cmdq(&data_array,1,1);
   
   
    //{0x11,  0,  {}},                        //Sleep out
    data_array[0]=0x00110500;
    dsi_set_cmdq(&data_array,1,1);
    
    //{REGFLAG_DELAY, 120, {}},
    MDELAY(150);

    //{0x29,  0,  {}},                        //Display on
    data_array[0]=0x00290500;
    dsi_set_cmdq(&data_array,1,1);
    
    //{REGFLAG_DELAY, 20, {}},
    MDELAY(20);

    //{0x2C,  0,  {}},    //NO USE                      //Memory write

    //{REGFLAG_END_OF_TABLE, 0x00, {}}
}
#endif
static void lcm_init(void)
{

    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);

    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(50);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);

    push_table(nt35510_lcm_initialization_setting, sizeof(nt35510_lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    //nt35510_init_setting();
}



static void lcm_suspend(void)
{
    unsigned int data_array[16];

    data_array[0]=0x00280500; // Display Off
    dsi_set_cmdq(&data_array, 1, 1);
    MDELAY(20);
    data_array[0] = 0x00100500; // Sleep In
    dsi_set_cmdq(&data_array, 1, 1);
    MDELAY(150);

    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);

    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(50);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);

}


static void lcm_resume(void)
{
#if 0
    unsigned int data_array[16];

    data_array[0] = 0x00110500; // Sleep Out
    dsi_set_cmdq(&data_array, 1, 1);
    MDELAY(150);
    data_array[0] = 0x00290500; // Display On
    dsi_set_cmdq(&data_array, 1, 1);
    MDELAY(20);
#endif
    lcm_init();

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

static struct LCM_setting_table lcm_backlight_level_setting[] = {
    {0x51, 1, {0xFF}},
/*shorten the time of screen on*/
//    {0x53, 1, {0x2C}}, //close dimming
//    {0x55, 1, {0x00}}, //close cabc
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void lcm_setbacklight(unsigned int level)
{
#if 0
    unsigned int mapped_level = 0;
    unsigned int data_array[16];
    unsigned char buffer=0x66;
    unsigned int array[16];
    mapped_level = level *  6 /10 ;

    // Refresh value of backlight level.
    lcm_backlight_level_setting[0].para_list[0] = mapped_level;

#ifdef BUILD_LK
    printf("uboot:nt35510_lcm_setbacklight mapped_level = %d,level=%d\n",mapped_level,level);
#else
    printk("kernel:nt35510_lcm_setbacklight mapped_level = %d,level=%d\n",mapped_level,level);
#endif

    push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);
#endif
    unsigned int data_array[16];
    unsigned char backlight = level & 0xFF ;
    
    data_array[0] = 0x00511500 | backlight << 24; 
    dsi_set_cmdq(&data_array, 1, 1);
}

#ifdef BUILD_LK
extern void DSI_clk_HS_mode();
#endif

static unsigned int lcm_compare_id(void)
{
#if 0
    unsigned int id=0;
    unsigned char buffer[2];
    unsigned int array[16];


    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(25);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(50);

#ifdef BUILD_LK
    DSI_clk_HS_mode(1);//enable mipi clk into HS mode
#endif

    array[0]=0x00000508;
    dsi_set_cmdq(array, 1, 1);
    MDELAY(10);

#ifdef BUILD_LK
    DSI_clk_HS_mode(0);//disable mipi clk into HS mode
#endif


    array[0]=0x00063902;
    array[1]=0x52aa55f0;
    array[2]=0x00000108;
    dsi_set_cmdq(array, 3, 1);
    MDELAY(10);

    array[0] = 0x00083700;
    dsi_set_cmdq(array, 1, 1);

    //read_reg_v2(0x04, buffer, 3);//if read 0x04,should get 0x008000,that is both OK.
    read_reg_v2(0xc5, buffer,2);

    id = buffer[0]<<8 |buffer[1];

#ifdef BUILD_LK
    printf("%s, id = 0x%x \n", __func__, id);
#endif

    if(id == LCM_ID_NT35510)
        return 1;
    else
        return 0;
#else
    return ((which_lcd_module() ==  LCD_MODUL_ID) ? 1:0);
#endif


}

#ifndef BUILD_LK
#if 0
static unsigned int lcm_esd_check(void)
{

    unsigned char buffer_1[12];
    unsigned int array[16],i;

    for(i = 0; i < 12; i++)
      buffer_1[i] = 0x00;

    //---------------------------------
    // Set Maximum Return Size
    //---------------------------------
    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    //---------------------------------
    // Read [9Ch, 00h, ECC] + Error Report(4 Bytes)
    //---------------------------------
    read_reg_v2(0x0A, buffer_1, 7);

    printk(KERN_EMERG "start read(0x0A)\n");
    for(i = 0; i < 7; i++)
      printk(KERN_EMERG "buffer_1[%d]:0x%x \n",i,buffer_1[i]);
    printk(KERN_EMERG "end read(0x0A)\n");


    //---------------------------------
    // Set Maximum Return Size
    //---------------------------------
    array[0] = 0x00033700;
    dsi_set_cmdq(array, 1, 1);

    //---------------------------------
    // Clear D-PHY Buffer
    // Read [WC, WC, ECC, P1, P2, P3, CRC0, CRC1]+ Error Report(4 Bytes)
    //---------------------------------
    read_reg_v2(0xBC, buffer_1, 12);

    printk(KERN_EMERG "Clear D-PHY Buffer: read(0xBC)\n");
    for(i = 0; i < 12; i++)
      printk(KERN_EMERG "buffer_1[%d]:0x%x \n",i,buffer_1[i]);

    return 0;

}

static unsigned int lcm_esd_recover(void)
{
    printk(KERN_EMERG "lcm_esd_recover: nothing to do\n");

    return 0;
}
#endif
#endif


LCM_DRIVER nt35510_fwvga_lcm_drv =
{
    .name = "nt35510_dsi",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
#if defined(LCM_DSI_CMD_MODE)
    .update = lcm_update,
    .set_backlight	= lcm_setbacklight,
#endif
#ifndef BUILD_LK
    //.esd_check      = lcm_esd_check,
    //.esd_recover    = lcm_esd_recover,
#endif
};
