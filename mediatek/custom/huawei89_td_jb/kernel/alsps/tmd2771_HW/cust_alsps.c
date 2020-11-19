#include <linux/types.h>
#include <mach/mt_pm_ldo.h>
#include <cust_alsps.h>
static struct alsps_hw G520_cust_alsps_hw = {
    .i2c_num    = 3,
	.polling_mode_ps =0,
	.polling_mode_als =1,
    .power_id   = MT65XX_POWER_NONE,    /*LDO is not used*/
    .power_vol  = VOL_DEFAULT,          /*LDO is not used*/
    .i2c_addr   = {0x72, 0x48, 0x78, 0x00},
    .als_level  = {28,84,140,274,345,395,456},
    .als_value  = {10,255,320,640,1280,2600,10240},
    .ps_threshold = 900,
    .ps_threshold_high = 160,
    .ps_threshold_low = 120,
};
struct alsps_hw *get_cust_alsps_hw(void) 
{
    return &G520_cust_alsps_hw;
}
int TMD2771_CMM_PPCOUNT_VALUE = 0;
/* modify the als_ps sensor parameter */
int TMD2771_G520_CMM_PPCOUNT_VALUE = 0x04;
int ZOOM_TIME = 5;
int TMD2771_CMM_CONTROL_VALUE = 0x20;
