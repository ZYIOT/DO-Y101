// #include "bsp_hal.h"
#include "bsp_usart.h"
#include "bsp_dac.h"
#include "bsp_mcu.h"
#include "bsp_wdg.h"
#include "bsp_log.h"
#include "bsp_adc.h"
#ifndef GPIO_IIC_LIB
#include "bsp_ltc2450.h"
#else
#include "bsp_adc_dev.h"
#endif
#include "bsp_wdg.h"
#include "app.h"
#include "app_task.h"
#include "app_config.h"
#include "app_modbus.h"

// 版本更改时，修改board_config.h里对应的版本号的宏的值即可
#define NUM2STR(num)	#num
#define VER2STR(major,minor,revision)	NUM2STR(major)"."NUM2STR(minor)"."NUM2STR(revision)
#define VERSION_STR		VER2STR(APP_FIRMWARE_MAJOR, APP_FIRMWARE_MINOR, APP_FIRMWARE_REVISION)
#define APP_FIREWARE_NAME_STR		PN_CODE"_APP_V"VERSION_STR"_ADC"
uint8_t app_file_name_str[64] __attribute__((at(IAP_APPLICATION1_ADDRESS + 0x400))) = APP_FIREWARE_NAME_STR;
uint8_t compiler_time[128] __attribute__((at(IAP_APPLICATION1_ADDRESS + 0x440))) = __DATE__"-"__TIME__;

void APP_Version_init(void)
{
    BSP_LOG_debug("Hardware Version:%d.%d.%d\r\n", APP_HARDWARE_MAJOR, APP_HARDWARE_MINOR, APP_HARDWARE_REVISION);
    BSP_LOG_debug("Firmware Version:%d.%d.%d\r\n", APP_FIRMWARE_MAJOR, APP_FIRMWARE_MINOR, APP_FIRMWARE_REVISION);
}


void APP_get_flash_info(void)
{
    uint16_t size = BSP_FLASH_get_flash_size();
    uint32_t uid[3] = {0};
    BSP_FLASH_get_uid(uid);
    BSP_LOG_trace("flash size :%dKB\r\n", size);
    BSP_LOG_trace("UID:%08X %08x %08x\r\n", uid[2], uid[1], uid[0]);
}


int app_main(void)
{
    __enable_irq();
    __set_PRIMASK(0);
    __set_FAULTMASK(0);
    BSP_WDG_init();
    BSP_WDG_feed();
    
    DWT_init();
    BSP_UART_init();
    BSP_LOG_init();
    APP_MODBUS_init();
    BSP_WDG_feed();
    BSP_DAC_init();
    BSP_ADC_init();
   // BSP_LTC2450_init();
    APP_Version_init();
    APP_get_flash_info();
#ifdef ENABLE_FLASH_RDP
    APP_Flash_RDP_init(1);
#endif    
    BSP_WDG_feed();
    APP_TASK_init();
    __enable_irq();
    APP_TASK_run();
    return 0;
}
