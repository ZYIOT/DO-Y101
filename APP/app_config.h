#ifndef __APP_CONFIG_H
#define __APP_CONFIG_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app.h"
#include "bsp_flash.h"
#include "app_modbus.h"
#include "iap.h"

#define APP_CONFIG_SETTINGS_ADDRESS APPLICATION_USER_DATA_ADDRESS
#define APP_CONFIG_SETTINGS_LENGTH FLASH_PAGE_SIZE

    int APP_CONFIG_init(void);
    int APP_CONFIG_write_settings(void);
    int APP_CONFIG_read_settings(void);
    int APP_CONFIG_write_device_info(void);
    int APP_CONFIG_read_device_info(void);
    uint8_t APP_CONFIG_device_id(void);
#ifdef __cplusplus
}
#endif
#endif
