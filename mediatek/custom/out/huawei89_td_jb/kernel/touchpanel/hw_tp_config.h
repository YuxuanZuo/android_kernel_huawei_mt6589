/******************************************************************************
  Copyright ,Huawei Tech. Co., Ltd.
  File name:       hw_tp_config.h
  Author:       fengyonghui
  Description:  This file include Touch fw and config for solving touch performance. 
  Others:         NA
******************************************************************************/
#include <linux/types.h>
#include <linux/list.h>
#define BIT_IS_BOOTLOADER_MODE 0x40
enum rmi_dev_status_code
{
    RMI_DEV_NO_ERROR        = 0x00,  //No Error
    RMI_DEV_RESET_OCCURRED  = 0x01,  //Reset occurred
    RMI_DEV_INVALID_CFG     = 0x02,  //Invalid Configuration
    RMI_DEV_DEVICE_FAILURE  = 0x03,  //Device Failure
    RMI_DEV_CFG_CRC_FAILURE = 0x04,  //Configuration CRC Failure
    RMI_DEV_FW_CRC_FAILURE  = 0x05,  //Firmware CRC Failure
    RMI_DEV_CRC_IN_PROGRESS = 0x06,  //CRC In Progress
};
#define SYN_CONFIG_SIZE (32 * 16)
uint8_t *get_tp_config(int moudle_id,int ic_number);
int32_t  get_fw_pr_version(int moudle_id,int ic_number);
uint8_t* get_tp_lockdown_config(void);
enum TP_TYPE get_tp_type(void);
