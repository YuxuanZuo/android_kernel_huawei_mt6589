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
#ifdef BUILD_LK
#else
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(540)
#define FRAME_HEIGHT 										(960)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									1

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   


static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {

	

};



static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
    	{REGFLAG_DELAY, 120, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
    	{REGFLAG_DELAY, 40, {}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
    	{REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
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
#if 0
static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));

		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

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
		//params->dsi.packet_size=256;

		// Video mode setting
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=540*3;

		params->dsi.vertical_sync_active=3;
		params->dsi.vertical_backporch=12;
		params->dsi.vertical_frontporch=2;
		params->dsi.vertical_active_line=960;

		params->dsi.line_byte=2048;		// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=146;
		params->dsi.horizontal_frontporch_byte=146;
		params->dsi.rgb_byte=(540*3+6);

		params->dsi.horizontal_sync_active_word_count=20;
		params->dsi.horizontal_backporch_word_count=140;
		params->dsi.horizontal_frontporch_word_count=140;

		// Bit rate calculation
		params->dsi.pll_div1=1;		 // div1=0,1,2,3;div1_real=1,2,4,4
		params->dsi.pll_div2=1;		 // div2=0,1,2,3;div1_real=1,2,4,4
		params->dsi.fbk_div=38;           //fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

}
#else
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


		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.vertical_active_line				= FRAME_HEIGHT;

		params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

		params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4
		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4
		params->dsi.fbk_div =30;		// fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)		


}


#endif

static void lcm_init(void)
{
unsigned int data_array[16];    
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(5);
    SET_RESET_PIN(1);
    MDELAY(10);

data_array[0]=0x00053902;
data_array[1]=0x2555AAFF;
data_array[2]=0x00000001;
dsi_set_cmdq(&data_array,3,1);

data_array[0]=0x00023902;
data_array[1]=0x000000F5;
dsi_set_cmdq(&data_array,2,1);


data_array[0]=0x00063902;
data_array[1]=0x52AA55F0;
data_array[2]=0x00000008;
dsi_set_cmdq(&data_array,3,1);


data_array[0]=0x70C71500;
dsi_set_cmdq(&data_array,1,1);


data_array[0]=0x000c3902;
data_array[1]=0x1B1B00CA;
data_array[2]=0x1B1B1B1B;
data_array[3]=0x00000202;
dsi_set_cmdq(&data_array,4,1);

data_array[0]=0x05BA1500;
dsi_set_cmdq(&data_array,1,1);


data_array[0]=0x00073902;
data_array[1]=0x0D0641C9;
data_array[2]=0x00173A;
dsi_set_cmdq(&data_array,3,1);


data_array[0]=0x00043902;
data_array[1]=0x0006ECB1;
dsi_set_cmdq(&data_array,2,1);


data_array[0]=0x00043902;
data_array[1]=0x000000BC;
dsi_set_cmdq(&data_array,2,1);

data_array[0]=0x00033902;
data_array[1]=0x2272B7; 
dsi_set_cmdq(&data_array,2,1);


data_array[0]=0x00053902;
data_array[1]=0x020201B8;
data_array[2]=0x00;
dsi_set_cmdq(&data_array,3,1);


data_array[0]=0x00043902;
data_array[1]=0x530353BB;
dsi_set_cmdq(&data_array,2,1);

data_array[0]=0x0BB61500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x773A1500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0xD0361500;
dsi_set_cmdq(&data_array,1,1);

data_array[0]=0x00063902;
data_array[1]=0x52AA55F0;
data_array[2]=0x0108;
dsi_set_cmdq(&data_array,3,1);

data_array[0]=0x00351500;

dsi_set_cmdq(&data_array,1,1);

#if 0
{0x000D1,0x04,0},//gamma25
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00074,0x02,0},
{0x00000,0x02,0},
{0x000C9,0x02,0},
{0x00001,0x02,0},
{0x00005,0x02,0},
{0x00001,0x02,0},
{0x00016,0x02,0},
{0x00001,0x02,0},
{0x0003E,0x02,0},
{0x00001,0x02,0},
{0x00066,0x02,0},
{0x00001,0x02,0},
{0x00095,0x02,0},//8
{0x000D2,0x04,0},//D2
{0x00001,0x02,0},
{0x000B8,0x02,0},//BA
{0x00001,0x02,0},
{0x000E5,0x02,0},//E5
{0x00002,0x02,0},
{0x0001B,0x02,0},//1D
{0x00002,0x02,0},
{0x0005D,0x02,0},//12 5f
{0x00002,0x02,0},
{0x00093,0x02,0},
{0x00002,0x02,0},
{0x00095,0x02,0},
{0x00002,0x02,0},
{0x000C4,0x02,0},
{0x00002,0x02,0},
{0x000F8,0x02,0},
{0x000D3,0x04,0},//D3
{0x00003,0x02,0},
{0x00018,0x02,0},
{0x00003,0x02,0},
{0x00044,0x02,0},
{0x00003,0x02,0},
{0x00060,0x02,0},
{0x00003,0x02,0},
{0x00082,0x02,0},
{0x00003,0x02,0},
{0x00098,0x02,0},
{0x00003,0x02,0},
{0x000B5,0x02,0},
{0x00003,0x02,0},
{0x000DB,0x02,0},
{0x00003,0x02,0},
{0x000F0,0x02,0},
{0x000D4,0x04,0},//D4
{0x00003,0x02,0},
{0x000F7,0x02,0},
{0x00003,0x02,0},
{0x000F8,0x02,0},
{0x000D5,0x04,0},//gamma D5
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00074,0x02,0},
{0x00000,0x02,0},
{0x000C9,0x02,0},
{0x00001,0x02,0},
{0x00005,0x02,0},
{0x00001,0x02,0},
{0x00016,0x02,0},
{0x00001,0x02,0},
{0x0003E,0x02,0},
{0x00001,0x02,0},
{0x00066,0x02,0},
{0x00001,0x02,0},
{0x00095,0x02,0},
{0x000D6,0x04,0},//D6
{0x00001,0x02,0},
{0x000B8,0x02,0},//BA
{0x00001,0x02,0},
{0x000E5,0x02,0},//E5
{0x00002,0x02,0},
{0x0001B,0x02,0},//1D
{0x00002,0x02,0},
{0x0005D,0x02,0},//12 5f
{0x00002,0x02,0},
{0x00093,0x02,0},
{0x00002,0x02,0},
{0x00095,0x02,0},
{0x00002,0x02,0},
{0x000C4,0x02,0},
{0x00002,0x02,0},
{0x000F8,0x02,0},
{0x000D7,0x04,0},//D7
{0x00003,0x02,0},
{0x00018,0x02,0},
{0x00003,0x02,0},
{0x00044,0x02,0},
{0x00003,0x02,0},
{0x00060,0x02,0},
{0x00003,0x02,0},
{0x00082,0x02,0},
{0x00003,0x02,0},
{0x00098,0x02,0},
{0x00003,0x02,0},
{0x000B5,0x02,0},
{0x00003,0x02,0},
{0x000DB,0x02,0},
{0x00003,0x02,0},
{0x000F0,0x02,0},
{0x000D8,0x04,0},//D8
{0x00003,0x02,0},
{0x000F7,0x02,0},
{0x00003,0x02,0},
{0x000F8,0x02,0},
{0x000D9,0x04,0},//gamma D9
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00074,0x02,0},
{0x00000,0x02,0},
{0x000C9,0x02,0},
{0x00001,0x02,0},
{0x00005,0x02,0},
{0x00001,0x02,0},
{0x00016,0x02,0},
{0x00001,0x02,0},
{0x0003E,0x02,0},
{0x00001,0x02,0},
{0x00066,0x02,0},
{0x00001,0x02,0},
{0x00095,0x02,0},
{0x000DD,0x04,0},//DD
{0x00001,0x02,0},
{0x000B8,0x02,0},//BA
{0x00001,0x02,0},
{0x000E5,0x02,0},//E5
{0x00002,0x02,0},
{0x0001B,0x02,0},//1D
{0x00002,0x02,0},
{0x0005D,0x02,0},//12 5f
{0x00002,0x02,0},
{0x00093,0x02,0},
{0x00002,0x02,0},
{0x00095,0x02,0},
{0x00002,0x02,0},
{0x000C4,0x02,0},
{0x00002,0x02,0},
{0x000F8,0x02,0},
{0x000DE,0x04,0},//DE
{0x00003,0x02,0},
{0x00018,0x02,0},
{0x00003,0x02,0},
{0x00044,0x02,0},
{0x00003,0x02,0},
{0x00060,0x02,0},
{0x00003,0x02,0},
{0x00082,0x02,0},
{0x00003,0x02,0},
{0x00098,0x02,0},
{0x00003,0x02,0},
{0x000B5,0x02,0},
{0x00003,0x02,0},
{0x000DB,0x02,0},
{0x00003,0x02,0},
{0x000F0,0x02,0},
{0x000DF,0x04,0},//DF
{0x00003,0x02,0},
{0x000F7,0x02,0},
{0x00003,0x02,0},
{0x000F8,0x02,0},
{0x000E0,0x04,0},//gamma E0
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00074,0x02,0},
{0x00000,0x02,0},
{0x000C9,0x02,0},
{0x00001,0x02,0},
{0x00005,0x02,0},
{0x00001,0x02,0},
{0x00016,0x02,0},
{0x00001,0x02,0},
{0x0003E,0x02,0},
{0x00001,0x02,0},
{0x00066,0x02,0},
{0x00001,0x02,0},
{0x00095,0x02,0},
{0x000E1,0x04,0},//E1
{0x00001,0x02,0},
{0x000B8,0x02,0},//BA
{0x00001,0x02,0},
{0x000E5,0x02,0},//E5
{0x00002,0x02,0},
{0x0001B,0x02,0},//1D
{0x00002,0x02,0},
{0x0005D,0x02,0},//12 5f
{0x00002,0x02,0},
{0x00093,0x02,0},
{0x00002,0x02,0},
{0x00095,0x02,0},
{0x00002,0x02,0},
{0x000C4,0x02,0},
{0x00002,0x02,0},
{0x000F8,0x02,0},
{0x000E2,0x04,0},//E2
{0x00003,0x02,0},
{0x00018,0x02,0},
{0x00003,0x02,0},
{0x00044,0x02,0},
{0x00003,0x02,0},
{0x00060,0x02,0},
{0x00003,0x02,0},
{0x00082,0x02,0},
{0x00003,0x02,0},
{0x00098,0x02,0},
{0x00003,0x02,0},
{0x000B5,0x02,0},
{0x00003,0x02,0},
{0x000DB,0x02,0},
{0x00003,0x02,0},
{0x000F0,0x02,0},
{0x000E3,0x04,0},//E3
{0x00003,0x02,0},
{0x000F7,0x02,0},
{0x00003,0x02,0},
{0x000F8,0x02,0},
{0x000E4,0x04,0},//gamma E4
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00074,0x02,0},
{0x00000,0x02,0},
{0x000C9,0x02,0},
{0x00001,0x02,0},
{0x00005,0x02,0},
{0x00001,0x02,0},
{0x00016,0x02,0},
{0x00001,0x02,0},
{0x0003E,0x02,0},
{0x00001,0x02,0},
{0x00066,0x02,0},
{0x00001,0x02,0},
{0x00095,0x02,0},
{0x000E5,0x04,0},//E5
{0x00001,0x02,0},
{0x000B8,0x02,0},//BA
{0x00001,0x02,0},
{0x000E5,0x02,0},//E5
{0x00002,0x02,0},
{0x0001B,0x02,0},//1D
{0x00002,0x02,0},
{0x0005D,0x02,0},//12 5f
{0x00002,0x02,0},
{0x00093,0x02,0},
{0x00002,0x02,0},
{0x00095,0x02,0},
{0x00002,0x02,0},
{0x000C4,0x02,0},
{0x00002,0x02,0},
{0x000F8,0x02,0},
{0x000E6,0x04,0},//E6
{0x00003,0x02,0},
{0x00018,0x02,0},
{0x00003,0x02,0},
{0x00044,0x02,0},
{0x00003,0x02,0},
{0x00060,0x02,0},
{0x00003,0x02,0},
{0x00082,0x02,0},
{0x00003,0x02,0},
{0x00098,0x02,0},
{0x00003,0x02,0},
{0x000B5,0x02,0},
{0x00003,0x02,0},
{0x000DB,0x02,0},
{0x00003,0x02,0},
{0x000F0,0x02,0},
{0x000E7,0x04,0},//E7
{0x00003,0x02,0},
{0x000F7,0x02,0},
{0x00003,0x02,0},
{0x000F8,0x02,0},
{0x000E8,0x04,0},//gamma E8
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00000,0x02,0},
{0x00074,0x02,0},
{0x00000,0x02,0},
{0x000C9,0x02,0},
{0x00001,0x02,0},
{0x00005,0x02,0},
{0x00001,0x02,0},
{0x00016,0x02,0},
{0x00001,0x02,0},
{0x0003E,0x02,0},
{0x00001,0x02,0},
{0x00066,0x02,0},
{0x00001,0x02,0},
{0x00095,0x02,0},
{0x000E9,0x04,0},//E9
{0x00001,0x02,0},
{0x000B8,0x02,0},//BA
{0x00001,0x02,0},
{0x000E5,0x02,0},//E5
{0x00002,0x02,0},
{0x0001B,0x02,0},//1D
{0x00002,0x02,0},
{0x0005D,0x02,0},//12 5f
{0x00002,0x02,0},
{0x00093,0x02,0},
{0x00002,0x02,0},
{0x00095,0x02,0},
{0x00002,0x02,0},
{0x000C4,0x02,0},
{0x00002,0x02,0},
{0x000F8,0x02,0},
{0x000EA,0x04,0},//EA
{0x00003,0x02,0},
{0x00018,0x02,0},
{0x00003,0x02,0},
{0x00044,0x02,0},
{0x00003,0x02,0},
{0x00060,0x02,0},
{0x00003,0x02,0},
{0x00082,0x02,0},
{0x00003,0x02,0},
{0x00098,0x02,0},
{0x00003,0x02,0},
{0x000B5,0x02,0},
{0x00003,0x02,0},
{0x000DB,0x02,0},
{0x00003,0x02,0},
{0x000F0,0x02,0},
{0x000EB,0x04,0},//EB
{0x00003,0x02,0},
{0x000F7,0x02,0},
{0x00003,0x02,0},
{0x000F8,0x02,0},
#endif

data_array[0]=0x0AB01500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x54B61500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x0AB11500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x34B71500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x00B21500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x30B81500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x0DB31500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x24B91500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x08B41500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x14BA1500;
dsi_set_cmdq(&data_array,1,1);


data_array[0]=0x00043902;
data_array[1]=0x007800BC; 
dsi_set_cmdq(&data_array,2,1);


data_array[0]=0x00043902;
data_array[1]=0x007800BD;
dsi_set_cmdq(&data_array,2,1);

data_array[0]=0x45BE1500; 
dsi_set_cmdq(&data_array,1,1);

data_array[0]=0x00361500;
dsi_set_cmdq(&data_array,1,1);
data_array[0]=0x00111500;
dsi_set_cmdq(&data_array,1,1);
MDELAY(100);

data_array[0]=0x00291500;
dsi_set_cmdq(&data_array,1,1);
MDELAY(100);



}


static void lcm_suspend(void)
{
	   unsigned int data_array[16];
#ifdef BUILD_LK
#else
printk("zhibin enter lcm suspend\n");
#endif
	   
          data_array[0]=0x00280500;
          dsi_set_cmdq(&data_array,1,1);
              MDELAY(10);
          data_array[0]=0x00100500;
          dsi_set_cmdq(&data_array,1,1);
              MDELAY(100);	   
#ifdef BUILD_LK
#else
printk("zhibin exit lcm suspend\n");
#endif
			  
          
}


static void lcm_resume(void)
{
	   unsigned int data_array[16];
#ifdef BUILD_LK
#else
printk("zhibin enter lcm resume\n");
#endif
	   
          data_array[0]=0x00110500;
          dsi_set_cmdq(&data_array,1,1);
              MDELAY(100);
          data_array[0]=0x00290500;
          dsi_set_cmdq(&data_array,1,1);
              MDELAY(10);
#ifdef BUILD_LK
#else
printk("zhibin exit lcm resume\n ");
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

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508;//HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

	

}




static void lcm_setbacklight(unsigned int level)
{


}


static void lcm_setpwm(unsigned int divider)
{
	// TBD
}

static unsigned int lcm_compare_id(void)
{

		int   array[4];
		char  buffer[3];
		char  id0=0;
		char  id1=0;
		char  id2=0;


		SET_RESET_PIN(0);
		MDELAY(200);
		SET_RESET_PIN(1);
		MDELAY(200);
		
	array[0] = 0x00083700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x04,buffer, 3);
	
	id0 = buffer[0]; //should be 0x00
	id1 = buffer[1];//should be 0x80
	id2 = buffer[2];//should be 0x00
	
	return 0;


}

static unsigned int lcm_getpwm(unsigned int divider)
{
	// ref freq = 15MHz, B0h setting 0x80, so 80.6% * freq is pwm_clk;
	// pwm_clk / 255 / 2(lcm_setpwm() 6th params) = pwm_duration = 23706
	unsigned int pwm_clk = 23706 / (1<<divider);
	return pwm_clk;
}
LCM_DRIVER nt35516_qhd_dsi_cmd_tianma_lcm_drv =
{
       .name			= "nt35516_qhd_dsi_cmd_tianma",
	.compare_id     = lcm_compare_id,
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
        //.set_backlight	= lcm_setbacklight,
		//.set_pwm        = lcm_setpwm,
		//.get_pwm        = lcm_getpwm,
        .update         = lcm_update
#endif
    };
