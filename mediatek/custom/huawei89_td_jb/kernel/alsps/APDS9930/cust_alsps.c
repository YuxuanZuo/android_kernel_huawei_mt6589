#include <linux/types.h>
#include <mach/mt_pm_ldo.h>
#include <cust_alsps.h>
//#include <mach/mt6577_pm_ldo.h>

static struct alsps_hw cust_alsps_hw = {
    .i2c_num    = 3,
	.polling_mode_ps =0,
	.polling_mode_als =1,
    .power_id   = MT65XX_POWER_NONE,    /*LDO is not used*/
    .power_vol  = VOL_DEFAULT,          /*LDO is not used*/
    .i2c_addr   = {0x72, 0x48, 0x78, 0x00},
    .als_level  = {28,84,140,274,345,395,456},
    .als_value  = {10,255,320,640,1280,2600,10240},
    .ps_threshold_high = 270, 
    .ps_threshold_low = 130, 
    .ps_threshold = 900,
};
struct alsps_hw *get_cust_alsps_hw(void) {
    return &cust_alsps_hw;
}
int APDS9930_CMM_PPCOUNT_VALUE = 0x04;
int APDS9930_CMM_CONTROL_VALUE = 0xE0;
int ZOOM_TIME = 4;
