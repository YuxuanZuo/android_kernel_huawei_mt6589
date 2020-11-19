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

#define REGFLAG_DELAY 0XFD
#define REGFLAG_END_OF_TABLE 0xFE // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE 1
#define LCM_ID 0x8009

#define LCD_MODUL_ID (0x02)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE; ///only for ESD test



// ---------------------------------------------------------------------------
// Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
// Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size) 
static struct LCM_setting_table {
unsigned char cmd;
unsigned char count;
unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {

    /*
    Note :

    Data ID will depends on the following rule.

    count of parameters > 1 => Data ID = 0x39
    count of parameters = 1 => Data ID = 0x15
    count of parameters = 0 => Data ID = 0x05

    Structure Format :

    {DCS command, count of parameters, {parameter list}}
    {REGFLAG_DELAY, milliseconds of time, {}},

    ...

    Setting ending by predefined flag

    {REGFLAG_END_OF_TABLE, 0x00, {}}
    */
    {0x00,	1,	{0x00}},
    {0xFF,	3,	{0x80,0x09,0x01}},
    {0x00,	1,	{0x80}},
    {0xFF,	2,	{0x80,0x09}},
    {REGFLAG_DELAY, 1, {}},
    {0x11,	0,	{}},
    {REGFLAG_DELAY, 200, {}},
    {0x00,	1,	{0x80}},
    {0xF5,	12,	{0x01,0x18,0x02,0x18,
                         0x10,0x18,0x02,0x18,
                         0x0E,0x18,0x0F,0x20}},

    {0x00,	1,	{0x90}},

    {0xF5,	10,	{0x02,0x18,
                         0x08,0x18,0x06,0x18,
                         0x0D,0x18,0x0B,0x18}},
    {0x00,	1,	{0xA0}},
    {0xF5,	8,	{0x10,0x18,
                         0x01,0x18,0x14,0x18,
                         0x14,0x18}},
    {0x00,	1,	{0xB0}},
    {0xF5,	12,	{0x14,0x18,0x12,0x18,
                         0x13,0x18,0x11,0x18,
                         0x13,0x18,0x00,0x00}},
    /*00h=>1 dot inversion 11h=>2 dot inversion 55h=>column inversion*/
    {0x00,	1,	{0xB4}},
    {0xC0,	1,	{0x55}},
    {0x00,	1,	{0x82}},
    {0xC5,	1,	{0xA3}},
    {0x00,	1,	{0x90}},
    {0xC5,	2,	{0xD6,0x76}},
    {0x00,	1,	{0x00}},
    {0xD8,	2,	{0xA7,0xA7}},
    /*vcom value delete*/
    /*{0x00,	1,	{0x00}},
    {0xD9,	1,	{0x74}},
    */
    {0x00,	1,	{0x81}},
    {0xC1,	1,	{0x55}},  //0x55-60Hz
    {0x00,	1,	{0xA1}},
    {0xC1,	1,	{0x08}},
    {0x00,	1,	{0xA3}},
    {0xC0,	1,	{0x1B}},
    {0x00,	1,	{0x81}},
    {0xC4,	1,	{0x83}},
    {0x00,	1,	{0x92}},
    {0xC5,	1,	{0x01}},
    {0x00,	1,	{0xB1}},
    {0xC5,	1,	{0xA9}},
    {0x00,	1,	{0x80}},
    {0xCE,	6,	{0x84,0x03,0x00,
                         0x83,0x03,0x00}},
    /*source output levels during porch and non-display*/
    {0x00,1,{0x80}},
    {0xC4,1,{0x30}},
    {0x00,	1,	{0x90}},
    {0xB3,	1,	{0x02}},
    {0x00,	1,	{0x92}},
    {0xB3,	1,	{0x45}},

    {0x00,	1,	{0x80}},
    {0xC0,	5,	{0x00,0x57,0x00,0x15,0x15}},

    {0x00,	1,	{0x90}},
    {0xCE,	6,	{0x33,0x5D,0x00,
                         0x33,0x5E,0x00}},
    {0x00,	1,	{0xA0}},
    {0xCE,	14,	{0x38,0x02,0x03,0x57,
                         0x00,0x00,0x00,0x38,
                         0x01,0x03,0x58,0x00,
                         0x00,0x00}},
    {0x00,	1,	{0xB0}},
    {0xCE,	14,	{0x38,0x00,0x03,0x59,
                         0x00,0x00,0x00,0x30,
                         0x00,0x03,0x5A,0x00,
                         0x00,0x00}},
    {0x00,	1,	{0xC0}},
    {0xCE,	14,	{0x30,0x01,0x03,0x5B,
                         0x00,0x00,0x00,0x30,
                         0x02,0x03,0x5C,0x00,
                         0x00,0x00}},
    {0x00,	1,	{0xD0}},
    {0xCE,	14,	{0x30,0x03,0x03,0x5D,
                         0x00,0x00,0x00,0x30,
                         0x04,0x03,0x5E,0x00,
                         0x00,0x00}},
    {0x00,	1,	{0xC7}},
    {0xCF,	1,	{0x00}},
    {0x00,	1,	{0xC0}},
    {0xCB,	15,	{0x00,0x00,0x00,0x00,
                         0x54,0x54,0x54,0x54,
                         0x00,0x54,0x00,0x54,
                         0x00,0x00,0x00}},
    {0x00,	1,	{0xD0}},
    {0xCB,	15,	{0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x54,0x54,0x54,
                         0x54,0x00,0x54}},
    {0x00,	1,	{0xE0}},
    {0xCB,	9,	{0x00,0x54,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00}},
    {0x00,	1,	{0x80}},
    {0xCC,	10,	{0x00,0x00,0x00,0x00,
                         0x0C,0x0A,0x10,0x0E,
                         0x00,0x02}},
    {0x00,	1,	{0x90}},
    {0xCC,	15,	{0x00,0x06,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x0B}},
    {0x00,	1,	{0xA0}},
    {0xCC,	15,	{0x09,0x0F,0x0D,0x00,
                         0x01,0x00,0x05,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00}},
    {0x00,	1,	{0xB0}},
    {0xCC,	10,	{0x00,0x00,0x00,0x00,
                         0x0d,0x0f,0x09,0x0b,
                         0x00,0x05}},
    {0x00,	1,	{0xc0}},
    {0xCC,	15,	{0x00,0x01,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x0e}},
    {0x00,	1,	{0x00}},
    {0x35,	1,	{0x00}},//Tearing Effect on
    {0x00,	1,	{0x00}},
    //{0x36,	1,	{0xC0}},
    {0x36,	1,	{0x40}},/*in reverse */
    {0x00,	1,	{0xD0}},
    {0xCC,	15,	{0x10,0x0a,0x0c,0x00,
                         0x06,0x00,0x02,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00}},
    /*gamma 2.2*/
/*    {0x00,	1,	{0x00}},
    {0xE1, 16, {0x00,0x13,0x19,0x0e,
                0x07,0x0e,0x0a,0x08,
                0x04,0x07,0x10,0x09,
                0x10,0x10,0x08,0x03}},
    {0x00, 1, {0x00}},
    {0xE2, 16, {0x00,0x13,0x19,0x0e,
                0x07,0x0e,0x0a,0x08,
                0x04,0x07,0x10,0x09,
                0x10,0x10,0x08,0x03}},
*/
    /*gamma 2.5*/
    {0x00,1,{0x00}},
    {0xE1, 16, {0x00,0x14,0x1A,0x0F,
                0x07,0x0e,0x09,0x07,
                0x07,0x0b,0x0f,0x07,
                0x0e,0x0c,0x06,0x03}},
    {0x00, 1, {0x00}},
    {0xE2, 16, {0x00,0x14,0x1a,0x0f,
                0x07,0x0e,0x09,0x07,
                0x07,0x0b,0x0f,0x07,
                0x0e,0x0c,0x06,0x03}},
    {0x00,	1,	{0x00}},
    {0x26,	1,	{0x00}},
    {0x00,	1,	{0x00}},
    {0x2B,	4,	{0x00,0x00,0x03,0x56}},
    {0x00,	1,	{0x00}},
    {0x29,	0,	{}},
    {REGFLAG_DELAY, 100, {}},

    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_set_window[] = {
    {0x2A, 4, {0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
    {0x2B, 4, {0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
     // Display off sequence
     {0x28, 1, {0x00}},
     {REGFLAG_DELAY, 20, {}},
     // Sleep Mode On
     {0x10, 1, {0x00}},
     {REGFLAG_DELAY, 120, {}},
     {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_level_setting[] = {
    {0x51, 1, {0xFF}},
    {0x53, 1, {0x24}},//close dimming
    {0x55, 1, {0x00}},//close cabc
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
// LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
    memset(params, 0, sizeof(LCM_PARAMS));

    params->type = LCM_TYPE_DSI;

    params->width = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;

    // enable tearing-free
    params->dbi.te_mode = LCM_DBI_TE_MODE_VSYNC_ONLY;//LCM_DBI_TE_MODE_VSYNC_ONLY;LCM_DBI_TE_MODE_DISABLED
    params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

    #if (LCM_DSI_CMD_MODE)
    params->dsi.mode = CMD_MODE;
    #else
    params->dsi.mode = SYNC_PULSE_VDO_MODE;
    #endif

    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_TWO_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    // Highly depends on LCD driver capability.
    // Not support in MT6573
    params->dsi.packet_size=256;

    // Video mode setting
    params->dsi.intermediat_buffer_num = 2;

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 3;
    params->dsi.vertical_backporch = 15;
    params->dsi.vertical_frontporch = 16;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 4;
    params->dsi.horizontal_backporch =20;
    params->dsi.horizontal_frontporch =20;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;

    params->dsi.pll_div1=1;
    params->dsi.pll_div2=1;
    params->dsi.fbk_div=34;
}

static unsigned int lcm_compare_id(void)
{
#if 0
    int array[4];
    char buffer[5];
    char id_high=0;
    char id_low=0;
    int id=0;

    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(25);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(100);

    array[0] = 0x00053700;// read id return two byte,version and id
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0xA1,buffer, 5);

    id_high = buffer[2]; //should be 0x80
    id_low = buffer[3]; //should be 0x09
    id = (id_high<<8)|id_low; //should be 0x8009

    #ifdef BUILD_LK
    printf("otm8009a uboot %s\n", __func__);
    printf("%s, id = 0x%08x\n", __func__, id);//should be 0x8009
    #else
    printk("otm8009a kernel %s\n", __func__);
    printk("%s, id = 0x%08x\n", __func__, id);//should be 0x8009
    #endif
    return (LCM_ID == id)?1:0;
#else
    return ((LCD_MODUL_ID == which_lcd_module())? 1:0);
#endif
}
#if 0
static void Otm8009a_fwvga_init(void)
{
     unsigned int data_array[8];
    /*
        {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
        {0xFF,	3,	{0x80,0x09,0x01}},
    */
    data_array[0] = 0x00043902;
    data_array[1] = 0x010980FF;
    dsi_set_cmdq(&data_array,2,1);
    /*
        {0x00,	1,	{0x80}},
    */
    data_array[0] = 0x80001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xFF,	2,	{0x80,0x09}},
    */
    data_array[0] = 0x00033902;
    data_array[1] = 0x000980FF;
    dsi_set_cmdq(&data_array,2,1);
    /*
    {REGFLAG_DELAY, 1, {}},
    */
    MDELAY(1);
    /*
    {0x11,	0,	{}},
    */
    data_array[0] = 0x00110500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {REGFLAG_DELAY, 200, {}},
    */
    MDELAY(200);
    /*
    {0x00,	1,	{0x80}},
    */
    data_array[0] = 0x80001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xF5,	12,	{0x01,0x18,0x02,0x18,
                         0x10,0x18,0x02,0x18,
                         0x0E,0x18,0x0F,0x20}},
    */
    data_array[0] = 0x000D3902;
    data_array[1] = 0x021801F5;
    data_array[2] = 0x02181018;
    data_array[3] = 0x0F180E18;
    data_array[4] = 0x00000020;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0x90}},
    */
    data_array[0] = 0x90001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xF5,	10,	{0x02,0x18,
                         0x08,0x18,0x06,0x18,
                         0x0D,0x18,0x0B,0x18}},
    */
    data_array[0] = 0x000B3902;
    data_array[1] = 0x081802F5;
    data_array[2] = 0x0D180618;
    data_array[3] = 0x00180B18;
    dsi_set_cmdq(&data_array,4,1);
    /*
    {0x00,	1,	{0xA0}},
    */
    data_array[0] = 0xA0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xF5,	8,	{0x10,0x18,
                         0x01,0x18,0x14,0x18,
                         0x14,0x18}},
    */
    data_array[0] = 0x00093902;
    data_array[1] = 0x011810F5;
    data_array[2] = 0x14181418;
    data_array[3] = 0x00000018;
    dsi_set_cmdq(&data_array,4,1);
    /*
    {0x00,	1,	{0xB0}},
    */
    data_array[0] = 0xB0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xF5,	12,	{0x14,0x18,0x12,0x18,
                         0x13,0x18,0x11,0x18,
                         0x13,0x18,0x00,0x00}},
    */
    data_array[0] = 0x000D3902;
    data_array[1] = 0x121814F5;
    data_array[2] = 0x11181318;
    data_array[3] = 0x00181318;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xB4}},
    */
    data_array[0] = 0xB4001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC0,	1,	{0x11}},
    */
    data_array[0] = 0x11C01500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x82}},
    */
    data_array[0] = 0x82001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC5,	1,	{0xA3}},
    */
    data_array[0] = 0xA3C51500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x90}},
    */
    data_array[0] = 0x90001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC5,	2,	{0xD6,0x76}},
    */
    data_array[0] = 0x00033902;
    data_array[1] = 0x0076D6C5;
    dsi_set_cmdq(&data_array,2,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xD8,	2,	{0xA7,0xA7}},
    */
    data_array[0] = 0x00033902;
    data_array[1] = 0x00A7A7D8;
    dsi_set_cmdq(&data_array,2,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xD9,	1,	{0x74}},
    */
    data_array[0] = 0x74D91500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x81}},
    */
    data_array[0] = 0x81001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC1,	1,	{0x66}},
    */
    data_array[0] = 0x66C11500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0xA1}},
    */
    data_array[0] = 0xA1001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC1,	1,	{0x08}},
    */
    data_array[0] = 0x08C11500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0xA3}},
    */
    data_array[0] = 0xA3001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC0,	1,	{0x1B}},
    */
    data_array[0] = 0x1BC01500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x81}},
    */
    data_array[0] = 0x81001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC4,	1,	{0x83}},
    */
    data_array[0] = 0x83C41500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x92}},
    */
    data_array[0] = 0x92001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC5,	1,	{0x01}},
    */
    data_array[0] = 0x01C51500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0xB1}},
    */
    data_array[0] = 0xB1001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC5,	1,	{0xA9}},
    */
    data_array[0] = 0xA9C51500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x80}},
    */
    data_array[0] = 0x80001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCE,	6,	{0x84,0x03,0x00,
                         0x83,0x03,0x00}},
   */
    data_array[0] = 0x00073902;
    data_array[1] = 0x000384CE;
    data_array[2] = 0x00000383;
    dsi_set_cmdq(&data_array,3,1);
    /*
    {0x00,	1,	{0x90}},
    */
    data_array[0] = 0x90001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xB3,	1,	{0x02}},
    */
    data_array[0] = 0x02B31500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x92}},
    */
    data_array[0] = 0x92001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xB3,	1,	{0x45}},
    */
    data_array[0] = 0x45B31500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x80}},
    */
    data_array[0] = 0x80001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xC0,	5,	{0x00,0x57,0x00,0x15,0x15}},
    */
    data_array[0] = 0x00063902;
    data_array[1] = 0x005700C0;
    data_array[2] = 0x00001515;
    dsi_set_cmdq(&data_array,3,1);
    /*
    {0x00,	1,	{0x90}},
    */
    data_array[0] = 0x90001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCE,	6,	{0x33,0x5D,0x00,
                         0x33,0x5E,0x00}},
    */
    data_array[0] = 0x00073902;
    data_array[1] = 0x00D533CE;
    data_array[2] = 0x00005E33;
    dsi_set_cmdq(&data_array,3,1);
    /*
    {0x00,	1,	{0xA0}},
    */
    data_array[0] = 0xA0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCE,	14,	{0x38,0x02,0x03,0x57,
                         0x00,0x00,0x00,0x38,
                         0x01,0x03,0x58,0x00,
                         0x00,0x00}},
    */
    data_array[0] = 0x000F3902;
    data_array[1] = 0x030238CE;
    data_array[2] = 0x00000057;
    data_array[3] = 0x58030138;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xB0}},
    */
    data_array[0] = 0xB0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCE,	14,	{0x38,0x00,0x03,0x59,
                         0x00,0x00,0x00,0x30,
                         0x00,0x03,0x5A,0x00,
                         0x00,0x00}},
    */
    data_array[0] = 0x000F3902;
    data_array[1] = 0x030038CE;
    data_array[2] = 0x00000059;
    data_array[3] = 0x5A030030;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xC0}},
    */
    data_array[0] = 0xC0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCE,	14,	{0x30,0x01,0x03,0x5B,
                         0x00,0x00,0x00,0x30,
                         0x02,0x03,0x5C,0x00,
                         0x00,0x00}},
    */
    data_array[0] = 0x000F3902;
    data_array[1] = 0x030130CE;
    data_array[2] = 0x0000005B;
    data_array[3] = 0x5C030230;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xD0}},
    */
    data_array[0] = 0xD0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCE,	14,	{0x30,0x03,0x03,0x5D,
                         0x00,0x00,0x00,0x30,
                         0x04,0x03,0x5E,0x00,
                         0x00,0x00}},
    */
    data_array[0] = 0x000F3902;
    data_array[1] = 0x030330CE;
    data_array[2] = 0x0000005D;
    data_array[3] = 0x5E030430;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xC7}},
    */
    data_array[0] = 0xC7001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCF,	1,	{0x00}},
    */
    data_array[0] = 0x00CF1500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0xC0}},
    */
    data_array[0] = 0xC0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCB,	15,	{0x00,0x00,0x00,0x00,
                         0x54,0x54,0x54,0x54,
                         0x00,0x54,0x00,0x54,
                         0x00,0x00,0x00}},
    */
    data_array[0] = 0x00103902;
    data_array[1] = 0x000000CB;
    data_array[2] = 0x54545400;
    data_array[3] = 0x00540054;
    data_array[4] = 0x00000054;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xD0}},
    */
    data_array[0] = 0xD0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCB,	15,	{0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x54,0x54,0x54,
                         0x54,0x00,0x54}},
    */
    data_array[0] = 0x00103902;
    data_array[1] = 0x000000CB;
    data_array[2] = 0x00000000;
    data_array[3] = 0x54540000;
    data_array[4] = 0x54005454;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xE0}},
    */
    data_array[0] = 0xE0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCB,	9,	{0x00,0x54,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00}},
    */
    data_array[0] = 0x000A3902;
    data_array[1] = 0x005400CB;
    data_array[2] = 0x00000000;
    data_array[3] = 0x00000000;
    dsi_set_cmdq(&data_array,4,1);
    /*
    {0x00,	1,	{0x80}},
    */
    data_array[0] = 0x80001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCC,	10,	{0x00,0x00,0x00,0x00,
                         0x0C,0x0A,0x10,0x0E,
                         0x00,0x02}},
    */
    data_array[0] = 0x000B3902;
    data_array[1] = 0x000000CC;
    data_array[2] = 0x100A0C00;
    data_array[3] = 0x0002000E;
    dsi_set_cmdq(&data_array,4,1);
    /*
    {0x00,	1,	{0x90}},
    */
    data_array[0] = 0x90001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCC,	15,	{0x00,0x06,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x0B}},
    */
    data_array[0] = 0x00103902;
    data_array[1] = 0x000600CC;
    data_array[2] = 0x00000000;
    data_array[3] = 0x00000000;
    data_array[4] = 0x0B000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xA0}},
    */
    data_array[0] = 0xA0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCC,	15,	{0x09,0x0F,0x0D,0x00,
                         0x01,0x00,0x05,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00}},
    */
    data_array[0] = 0x00103902;
    data_array[1] = 0x0D0F09CC;
    data_array[2] = 0x05000100;
    data_array[3] = 0x00000000;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0xB0}},
    */
    data_array[0] = 0xB0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCC,	10,	{0x00,0x00,0x00,0x00,
                         0x0d,0x0f,0x09,0x0b,
                         0x00,0x05}},
    */
    data_array[0] = 0x000B3902;
    data_array[1] = 0x000000CC;
    data_array[2] = 0x090F0D00;
    data_array[3] = 0x0005000B;
    dsi_set_cmdq(&data_array,4,1);
    /*
    {0x00,	1,	{0xc0}},
    */
    data_array[0] = 0xC0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCC,	15,	{0x00,0x01,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x0e}},
    */
    data_array[0] = 0x00103902;
    data_array[1] = 0x000100CC;
    data_array[2] = 0x00000000;
    data_array[3] = 0x00000000;
    data_array[4] = 0x0E000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x35,	1,	{0x00}},//Tearing Effect on
    */
    data_array[0] = 0x00351500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x36,	1,	{0xC0}},
    */
    data_array[0] = 0xC0361500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    //{0x36,	1,	{0x40}},
    {0x00,	1,	{0xD0}},
    */
    data_array[0] = 0xD0001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xCC,	15,	{0x10,0x0a,0x0c,0x00,
                         0x06,0x00,0x02,0x00,
                         0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00}},
    */
    data_array[0] = 0x00103902;
    data_array[1] = 0x0C0A10CC;
    data_array[2] = 0x02000600;
    data_array[3] = 0x00000000;
    data_array[4] = 0x00000000;
    dsi_set_cmdq(&data_array,5,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xE1,	16,	{0x00,0x0a,0x11,0x0e,
                         0x07,0x0f,0x0b,0x0b,
                         0x04,0x07,0x0d,0x09,
                         0x10,0x0f,0x08,0x03}},
    */
    data_array[0] = 0x00113902;
    data_array[1] = 0x110A00E1;
    data_array[2] = 0x0B0F070E;
    data_array[3] = 0x0D07040B;
    data_array[4] = 0x080F1009;
    data_array[5] = 0x00000003;
    dsi_set_cmdq(&data_array,6,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0xE2,	16,	{0x00,0x0A,0x11,0x0e,
                         0x07,0x0f,0x0b,0x0b,
                         0x04,0x07,0x0d,0x09,
                         0x10,0x0f,0x08,0x03}},
    */
    data_array[0] = 0x00113902;
    data_array[1] = 0x110A00E2;
    data_array[2] = 0x0B0F070E;
    data_array[3] = 0x0D07040B;
    data_array[4] = 0x080F1009;
    data_array[5] = 0x00000003;
    dsi_set_cmdq(&data_array,6,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x26,	1,	{0x00}},
    */
    data_array[0] = 0x00161500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x2B,	4,	{0x00,0x00,0x03,0x56}},
    */
    data_array[0] = 0x00053902;
    data_array[1] = 0x0300002B;
    data_array[2] = 0x00000056;
    dsi_set_cmdq(&data_array,3,1);
    /*
    {0x00,	1,	{0x00}},
    */
    data_array[0] = 0x00001500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {0x29,	0,	{}},
    
    */
    
    data_array[0] = 0x00290500;
    dsi_set_cmdq(&data_array,1,1);
    /*
    {REGFLAG_DELAY, 100, {}},
    */
    MDELAY(100);
    /*
    {REGFLAG_END_OF_TABLE, 0x00, {}}
    */
}
#endif
static void lcm_init(void)
{
    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    /*1 0 1:high low hig*/
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);  //shm
    MDELAY(5);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(30);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(30);
    #ifdef BUILD_LK
        printf("uboot:otm8009a init\n");
    #else
        printk("kernel:otm8009a init\n");
    #endif
    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    //Otm8009a_fwvga_init();
}


static void lcm_suspend(void)
{
#if 0
    lcm_util.set_gpio_mode(GPIO_DISP_LRSTB_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_DISP_LRSTB_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(25);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
#endif

    #ifdef BUILD_LK
        printf("uboot:otm8009a lcm_suspend\n");
    #else
        printk("kernel:otm8009a lcm_suspend\n");
    #endif
    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

}

static void lcm_resume(void)
{
    #ifdef BUILD_LK
        printf("uboot:otm8009a lcm_resume\n");
    #else
        printk("kernel:otm8009a lcm_resume\n");
    #endif
    //lcm_init();

    push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
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

static void lcm_setbacklight(unsigned int level)
{

    unsigned int mapped_level = 0;

    //for LGE backlight IC mapping table
    if(level > 255)
    level = 255;
    mapped_level = level * 7 / 10;//to reduce power

    // Refresh value of backlight level.
    lcm_backlight_level_setting[0].para_list[0] = mapped_level;

#ifdef BUILD_LK
    printf("uboot:otm8009a_lcm_setbacklight mapped_level = %d,level=%d\n",mapped_level,level);
#else
    printk("kernel:otm8009a_lcm_setbacklight mapped_level = %d,level=%d\n",mapped_level,level);
#endif

    push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);

}

static unsigned int lcm_esd_check(void)
{
    #ifndef BUILD_LK
    if(lcm_esd_test)
    {
        lcm_esd_test = FALSE;
        return TRUE;
    }

    if(read_reg(0x0A) == 0x9C)
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

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);

    push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(10);

    dsi_set_cmdq_V2(0x35, 1, &para, 1); ///enable TE
    MDELAY(10);

    return TRUE;
}


LCM_DRIVER otm8009a_fwvga_lcm_drv =
{
    .name = "cmi_otm8009a",
    .set_util_funcs = lcm_set_util_funcs,
    .compare_id = lcm_compare_id,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    #if (LCM_DSI_CMD_MODE)
    .set_backlight = lcm_setbacklight,
    //.esd_check = lcm_esd_check,
    //.esd_recover = lcm_esd_recover,
    .update = lcm_update,
    #endif
};
