#ifndef TOUCHPANEL_H__
#define TOUCHPANEL_H__

/* Pre-defined definition */

/* Register */
//#define FD_ADDR_MAX    	0xE9
//#define FD_ADDR_MIN    	0xDD
//#define FD_BYTE_COUNT 	6

#define CUSTOM_MAX_WIDTH (480)
#define CUSTOM_MAX_HEIGHT (800)

//#define TPD_UPDATE_FIRMWARE

#define TPD_HAVE_BUTTON
#define TPD_BUTTON_HEIGH        (60)
#define TPD_KEY_COUNT           3
#define TPD_KEYS                { KEY_BACK, KEY_HOMEPAGE, KEY_MENU}
#define TPD_KEYS_DIM_WVGA		{{72,894,160,TPD_BUTTON_HEIGH},\
							{240,894,160,TPD_BUTTON_HEIGH},\
							{410,894,160,TPD_BUTTON_HEIGH}}

#define TPD_POWER_SOURCE         MT65XX_POWER_LDO_VGP4
#define LCD_X_WVGA           540
#define LCD_Y_WVGA           960


//#define TPD_HAVE_CALIBRATION
//#define TPD_CALIBRATION_MATRIX  {2680,0,0,0,2760,0,0,0};
//#define TPD_WARP_START
//#define TPD_WARP_END

//#define TPD_RESET_ISSUE_WORKAROUND
//#define TPD_MAX_RESET_COUNT 3
//#define TPD_WARP_Y(y) ( TPD_Y_RES - 1 - y )
//#define TPD_WARP_X(x) ( x )

#endif /* TOUCHPANEL_H__ */
