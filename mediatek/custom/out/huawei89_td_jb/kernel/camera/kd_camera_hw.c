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
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <asm/atomic.h>
#include <linux/xlog.h>

#include "kd_camera_hw.h"

#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_camera_feature.h"
/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[kd_camera_hw]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
//#define PK_DBG_FUNC(fmt, arg...)    xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg)
#define PK_DBG_FUNC(fmt, arg...)    printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define DEBUG_CAMERA_HW_K
#ifdef DEBUG_CAMERA_HW_K
#define PK_DBG PK_DBG_FUNC
#define PK_ERR(fmt, arg...)         xlog_printk(ANDROID_LOG_ERR, PFX , fmt, ##arg)
#define PK_XLOG_INFO(fmt, args...) \
                do {    \
                   xlog_printk(ANDROID_LOG_INFO, PFX , fmt, ##arg); \
                } while(0)
#else
#define PK_DBG(a,...)
#define PK_ERR(a,...)
#define PK_XLOG_INFO(fmt, args...)
#endif



int kdCISModulePowerOn(CAMERA_DUAL_CAMERA_SENSOR_ENUM SensorIdx, char *currSensorName, BOOL On, char* mode_name)
{
#if 0
u32 pinSetIdx = 0;//default main sensor

#define IDX_PS_CMRST 0
#define IDX_PS_CMPDN 4

#define IDX_PS_MODE 1
#define IDX_PS_ON   2
#define IDX_PS_OFF  3

#if 0//Altai
u32 pinSet[3][8] = {
                    //for main sensor
                    {GPIO_CAMERA_CMRST_PIN,
                        GPIO_CAMERA_CMRST_PIN_M_GPIO,   /* mode */
                        GPIO_OUT_ONE,                   /* ON state */
                        GPIO_OUT_ZERO,                  /* OFF state */
                     GPIO_CAMERA_CMPDN_PIN,
                        GPIO_CAMERA_CMPDN_PIN_M_GPIO,
                        GPIO_OUT_ZERO,
                        GPIO_OUT_ONE,
                    },
                    //for sub sensor
                    {GPIO_CAMERA_CMRST1_PIN,
                     GPIO_CAMERA_CMRST1_PIN_M_GPIO,
                        GPIO_OUT_ONE,
                        GPIO_OUT_ZERO,
                     GPIO_CAMERA_CMPDN1_PIN,
                        GPIO_CAMERA_CMPDN1_PIN_M_GPIO,
                        GPIO_OUT_ZERO,
                        GPIO_OUT_ONE,
                    },
                    //for main_2 sensor
                    {GPIO_CAMERA_2_CMRST_PIN,
                        GPIO_CAMERA_2_CMRST_PIN_M_GPIO,   /* mode */
                        GPIO_OUT_ONE,                   /* ON state */
                        GPIO_OUT_ZERO,                  /* OFF state */
                     GPIO_CAMERA_2_CMPDN_PIN,
                        GPIO_CAMERA_2_CMPDN_PIN_M_GPIO,
                        GPIO_OUT_ZERO,
                        GPIO_OUT_ONE,
                    }
                   };

#else//K89
u32 pinSet[3][8] = {
                    //for main sensor
                    {GPIO_CAMERA_CMRST_PIN,
                        GPIO_CAMERA_CMRST_PIN_M_GPIO,   /* mode */
                        GPIO_OUT_ONE,                   /* ON state */
                        GPIO_OUT_ZERO,                  /* OFF state */
                    GPIO_CAMERA_CMPDN_PIN,
                    GPIO_CAMERA_CMPDN_PIN_M_GPIO,
                    GPIO_OUT_ONE,
                    GPIO_OUT_ZERO,
                    },
                    //for sub sensor
                    {GPIO_CAMERA_CMRST1_PIN,
                     GPIO_CAMERA_CMRST1_PIN_M_GPIO,
                        GPIO_OUT_ONE,
                        GPIO_OUT_ZERO,
                     GPIO_CAMERA_CMPDN1_PIN,
                        GPIO_CAMERA_CMPDN1_PIN_M_GPIO,
                        GPIO_OUT_ONE,
                        GPIO_OUT_ZERO,
                    },
                    //for main_2 sensor
                    {GPIO_CAMERA_2_CMRST_PIN,
                        GPIO_CAMERA_2_CMRST_PIN_M_GPIO,   /* mode */
                        GPIO_OUT_ONE,                   /* ON state */
                        GPIO_OUT_ZERO,                  /* OFF state */
                     GPIO_CAMERA_2_CMPDN_PIN,
                        GPIO_CAMERA_2_CMPDN_PIN_M_GPIO,
                        GPIO_OUT_ZERO,
                        GPIO_OUT_ONE,
                    }
                   };


#endif




    if (DUAL_CAMERA_MAIN_SENSOR == SensorIdx){
        pinSetIdx = 0;
    }
    else if (DUAL_CAMERA_SUB_SENSOR == SensorIdx) {
        pinSetIdx = 1;
    }
    else if (DUAL_CAMERA_MAIN_2_SENSOR == SensorIdx) {
        //pinSetIdx = 2;
    } 
   printk("[kdCISModulePowerOn pinSetIdx = %d, On =$d \n",pinSetIdx,On);
    //pinSetIdx = 0;//JH test

    //power ON
    if (On) {

        if(DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {

            printk("Power on  main sensor \n");
            printk("Power on for G520 main sensor: currSensorName:%s\n",currSensorName);

            //AF_VCC
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
            //sub sensor inter stand by status
            //PWDN pin is high
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("powerdone pin is high\n");

            //reset pin is low
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}	
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("reset pin is low\n");

            //main sensor power on timing

            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) 
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(10);
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(1);

                //PDN pin
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            }


            mdelay(10);
            PK_DBG("[ON_general 1.8V]sensorIdx:%d \n",SensorIdx);
            //DOVDD
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                //return -EIO;
                //goto _kdCISModulePowerOn_exit_;
            }


            mdelay(10);
            //AVDD
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                //return -EIO;
                //goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);
            //DVDD
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                //return -EIO;
                //goto _kdCISModulePowerOn_exit_;
            }
            mdelay(10);


            //enable active sensor
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) 
            {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(10);
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_ON])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(1);

                //PDN pin
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_ON])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
            }

        }

        if (DUAL_CAMERA_SUB_SENSOR == SensorIdx)
        {
            //sub sensor power on timing
            //vio = 1.8
            //vdd = 1.8
            //vaa = 2.8
            printk("Power on  subsensor \n");
            printk("Power on for G520 subsensor: currSensorName:%s\n",currSensorName);

            //AF_VCC
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            //main sensor inter stand by status
            //please zhibin add it
             //Main PWDN pin is low		
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("powerdone pin is low\n");

            	//Main RST pin is low
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("reset pin is low\n");

            //PWDN pin is low		
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("powerdone pin is low\n");

            	//RST pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("reset pin is high\n");

            //DOVDD
            printk("IOVDD is 1.8v \n");
            PK_DBG("[ON_general 1.8V]sensorIdx:%d \n",SensorIdx);
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))
            {
               PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
            }
            mdelay(1);


            //AVDD
            printk("AVDD is 2.8v \n");
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))
            {
                	PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
            }
            mdelay(1);

            //DVDD
            printk("DVDD is 1.8v \n");
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))
            {
                 PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
            }
            mdelay(1);

            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("reset pin is low\n");

            printk("delay 10 ms\n");
            mdelay(10);

            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("reset pin is high\n"); 
        }
        mdelay(10);
    }
    else {//power OFF 
        if(DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {
            printk("Power off  main sensor \n");
            printk("Power off for G520 main sensor: currSensorName:%s\n",currSensorName);
                //main sensor power off timing
                 //PK_DBG("[OFF]sensorIdx:%d \n",SensorIdx);
            if (GPIO_CAMERA_INVALID != pinSet[pinSetIdx][IDX_PS_CMRST]) {
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_MODE])){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_mode(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_MODE])){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMRST],GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_dir(pinSet[pinSetIdx][IDX_PS_CMPDN],GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMRST],pinSet[pinSetIdx][IDX_PS_CMRST+IDX_PS_OFF])){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //low == reset sensor
                if(mt_set_gpio_out(pinSet[pinSetIdx][IDX_PS_CMPDN],pinSet[pinSetIdx][IDX_PS_CMPDN+IDX_PS_OFF])){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                 //return -EIO;
                 goto _kdCISModulePowerOn_exit_;
            }


        }
        if (DUAL_CAMERA_SUB_SENSOR == SensorIdx)
        {
            printk("Power off  subsensor \n");
            printk("Power off for G520 subsensor: currSensorName:%s\n",currSensorName);
            //sub sensor power off timing
            //PWDN pin is high
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");} 	
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("powerdone pin is high\n");

            //reset pin is low
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}		 
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}	
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} 	
            printk("reset pin is low\n");

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D,mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2, mode_name)) {
                PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                //return -EIO;
                goto _kdCISModulePowerOn_exit_;
            }
        }

        mdelay(20);
    }//

    return 0;

_kdCISModulePowerOn_exit_:
    return -EIO;

#endif

#if 1

#define S5K4E1 1
#define OV5647 2
#define IDNONE 4
    static int cameraType = IDNONE;

    printk("[kdCISModulePowerOn SensorIdx = %d, On =%d \n",SensorIdx,On);

    //power ON
    if (On) {

        if(DUAL_CAMERA_SUB_SENSOR == SensorIdx)
        {

            // Power on for sub sensor
            printk("Power on for G520 subsensor: currSensorName:%s\n",currSensorName);

            //Power on AF power to avoid ov5647 to pull down I2C
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
            printk("Set main pwd pin to zero !\n");


            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

            //enable sub sensor--PWD pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IOVDD=1.8 V
            {
              PK_DBG("[CAMERA SENSOR] Fail to enable IO power\n");
              goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD=2.8v
            {
              PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
              goto _kdCISModulePowerOn_exit_;
            }
            if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))//DVDD=1.8V
            {
              PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
              goto _kdCISModulePowerOn_exit_;
            }
 
            mdelay(1);
            //enable sub sensor--RESET pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
            mdelay(2);
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

            mdelay(10);

        }
        else if ( DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {
            printk("Power on for G520  mainsensor: currSensorName:%s\n",currSensorName);

            if ( currSensorName && (0 == strcmp(currSensorName,"s5k4e1gamipiraw")))
            {
                printk("G520 Power on for  : s5k4e1gamipiraw:\n");
                cameraType = S5K4E1;
                //disable sub sensor --Power down pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down
                
                //disable sub sensor--RESET pin.
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))//DVDD=1.8V
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD=2.8v
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IOVDD=1.8 V
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable IO power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))//AF power
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                mdelay(5); 

                //enable main sensor---RESET pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
                msleep(10);
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                mdelay(2);

                //enable AF --PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN, GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(12);

            }
            else if (currSensorName && (0 == strcmp(currSensorName,"ov5647mipiraw")))
            {

                printk("G510 Power on  for : ov5647:\n");
                cameraType = OV5647;

                //Power on AF power to avoid ov5647 to pull down I2C
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A2, VOL_2800,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                //disable sub sensor --Power down pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

                //disable sub sensor--RESET pin.
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                 if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D2, VOL_1800,mode_name))//IOVDD=1.8 V
                 {
                     PK_DBG("[CAMERA SENSOR] Fail to enable IO power\n");
                     goto _kdCISModulePowerOn_exit_;
                 }

                 if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_A, VOL_2800,mode_name))//AVDD=2.8v
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                if(TRUE != hwPowerOn(CAMERA_POWER_VCAM_D, VOL_1800,mode_name))//DVDD=1.8V
                {
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                mdelay(10);
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(5);   
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}
                mdelay(5);

                //enable AF --PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN,GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ONE)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                mdelay(20);
            }
            else
            {
                PK_DBG("[CAMERA SENSOR] Cannot identify sensor name!\n");
                cameraType = IDNONE;
            }
        }

    }
    else {//power OFF

        if(DUAL_CAMERA_SUB_SENSOR == SensorIdx)
        {
            printk("Power off for G520 subsensor: currSensorName:%s\n",currSensorName);

            //disable sub sensor--PWD pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

            //disable sub sensor--RST pin
            if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
            if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
            if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
            mdelay(2);

     
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {//DVDD=1.8V
                PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {//AVDD=2.8v
                PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                goto _kdCISModulePowerOn_exit_;
             }
            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name)){//IOVDD=1.8V
                PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                goto _kdCISModulePowerOn_exit_;
            }

            if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
            {
                PK_DBG("[CAMERA SENSOR] Fail to enable analog power\n");
                goto _kdCISModulePowerOn_exit_;
            }

        }
        else if ( DUAL_CAMERA_MAIN_SENSOR == SensorIdx)
        {
            printk("Power off for mainsensor \n");
            printk("Power off for G510 mainsensor: currSensorName:%s\n",currSensorName);
            if (S5K4E1 == cameraType)
            {
                 printk("Power off   for : S5K4E1:\n");

                //disable sub sensor--PWD pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

                //disable sub sensor--RST pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(2);


                //Disable AF--PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN,GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                //power down main sensor
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");} //high == power down lens module

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {//AVDD=2.8v
                    PK_DBG("[CAMERA SENSOR] Fail to OFF analog power\n");
                    goto _kdCISModulePowerOn_exit_;
                 }
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {//DVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to OFF digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }
                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name)){//IOVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to enable digital power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF  CAMERA_POWER_VCAM_A2 power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

            }
            else if (OV5647 == cameraType)
            {
                printk("Power off   for : ov5647mip:\n");

                //disable sub sensor--PWD pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN1_PIN,GPIO_CAMERA_CMPDN1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");} //high == power down

                //disable sub sensor--RST pin
                if(mt_set_gpio_mode(GPIO_CAMERA_CMRST1_PIN,GPIO_CAMERA_CMRST1_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMRST1_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMRST1_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}
                mdelay(2);

                //Disable AF--PWDN
                if(mt_set_gpio_mode(GPIO_CAMERA_AF_PWDN_PIN,GPIO_CAMERA_AF_PWDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_AF_PWDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_AF_PWDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}


                if(mt_set_gpio_mode(GPIO_CAMERA_CMPDN_PIN,GPIO_CAMERA_CMPDN_PIN_M_GPIO)){PK_DBG("[CAMERA SENSOR] set gpio mode failed!! \n");}
                if(mt_set_gpio_dir(GPIO_CAMERA_CMPDN_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA SENSOR] set gpio dir failed!! \n");}
                if(mt_set_gpio_out(GPIO_CAMERA_CMPDN_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA SENSOR] set gpio failed!! \n");}

                 if(mt_set_gpio_mode(GPIO_CAMERA_CMRST_PIN,GPIO_CAMERA_CMRST_PIN_M_GPIO)){PK_DBG("[CAMERA LENS] set gpio mode failed!! \n");}
                 if(mt_set_gpio_dir(GPIO_CAMERA_CMRST_PIN,GPIO_DIR_OUT)){PK_DBG("[CAMERA LENS] set gpio dir failed!! \n");}
                 if(mt_set_gpio_out(GPIO_CAMERA_CMRST_PIN,GPIO_OUT_ZERO)){PK_DBG("[CAMERA LENS] set gpio failed!! \n");}

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D, mode_name)) {//DVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to OFF CAMERA_POWER_VCAM_D power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A,mode_name)) {//AVDD=2.8v
                    PK_DBG("[CAMERA SENSOR] Fail to OFF CAMERA_POWER_VCAM_A power\n");
                    goto _kdCISModulePowerOn_exit_;
                 }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_D2,mode_name)){//IOVDD=1.8V
                    PK_DBG("[CAMERA SENSOR] Fail to OFF CAMERA_POWER_VCAM_D2 power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

                if(TRUE != hwPowerDown(CAMERA_POWER_VCAM_A2,mode_name))
                {
                    PK_DBG("[CAMERA SENSOR] Fail to OFF  CAMERA_POWER_VCAM_A2 power\n");
                    goto _kdCISModulePowerOn_exit_;
                }

            }

        }
        mdelay(10);
    }

    return 0;
_kdCISModulePowerOn_exit_:
    return -EIO;

#endif
}

EXPORT_SYMBOL(kdCISModulePowerOn);

