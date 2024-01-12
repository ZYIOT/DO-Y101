#ifndef __IAP_H
#define __IAP_H

#ifdef __cplusplus
extern "C"
{
#endif

    // Bootloader    108k   0800 0000 ~ 0801 AFFF  大小 0x1B000
    // APP1 settings 2k     0801 B000 ~ 0801 B7FF  大小 0x800
    // APP2 settings 2k     0801 B800 ~ 0801 BFFF  大小 0x800
    // User Data     16k    0801 C000 ~ 0801 FFFF  大小 0x4000
    // APP1          192K   0802 0000 ~ 0804 FFFF  大小 0x30000
    // APP2          192K   0805 0000 ~ 0807 FFFF  大小 0x30000

#define IAP_FLASH_BASE_ADDRESS  0x08000000
#define IAP_FLASH_LENGTH        0x80000
#define IAP_APPLICATION1_ADDRESS 0x08020000
#define IAP_APPLICATION2_ADDRESS 0x08050000
#define IAP_APPLICATION1_SETTINGS_ADDRESS 0x0801B000
#define IAP_APPLICATION2_SETTINGS_ADDRESS 0x0801B800
#define IAP_APPLICATION_LENGTH          0x8000 // 32k,值不能太大，否则可能导致STM32F103在写入flash时看门狗复位(ST:1.0s, GD:0.6s) 
#define IAP_APPLICATION_LENGTH_MAX      0x30000 // 192k 
#define IAP_APPLICATION_SETTINGS_LENGTH 0x800   // 2k 

#define APPLICATION_USER_DATA_ADDRESS   0x0801C000
#define APPLICATION_USER_DATA_LENGTH    0x4000  // 16k 


#include "bootloader.h"

// #define BOOTLOADER_flush_app1(addr, buffer, length) BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION1_ADDRESS, IAP_APPLICATION_LENGTH)
// #define BOOTLOADER_flush_app1_settings(addr, buffer, length) BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION1_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
// #define BOOTLOADER_erase_app1() BOOTLOADER_erase(IAP_APPLICATION1_ADDRESS, IAP_APPLICATION_LENGTH)
// #define BOOTLOADER_erase_app1_ex(addr)  BOOTLOADER_erase(addr + IAP_APPLICATION1_ADDRESS, IAP_APPLICATION_LENGTH)
// #define BOOTLOADER_erase_app1_settings() BOOTLOADER_erase(IAP_APPLICATION1_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
// #define BOOTLOADER_crc32_app1(length) BOOTLOADER_crc32(IAP_APPLICATION1_ADDRESS, length)

// #define BOOTLOADER_flush_app2(addr, buffer, length) BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION2_ADDRESS, IAP_APPLICATION_LENGTH)
// #define BOOTLOADER_flush_app2_settings(addr, buffer, length) BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION2_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
// #define BOOTLOADER_erase_app2() BOOTLOADER_erase(IAP_APPLICATION2_ADDRESS, IAP_APPLICATION_LENGTH)
// #define BOOTLOADER_erase_app2_ex(addr)  BOOTLOADER_erase(addr + IAP_APPLICATION2_ADDRESS, IAP_APPLICATION_LENGTH)
// #define BOOTLOADER_erase_app2_settings() BOOTLOADER_erase(IAP_APPLICATION2_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
// #define BOOTLOADER_crc32_app2(length) BOOTLOADER_crc32(IAP_APPLICATION2_ADDRESS, length)
// #define BOOTLOADER_add_crc32_app2(addr, length, crc32_in)   BOOTLOADER_add_crc32(addr + IAP_APPLICATION2_ADDRESS, length, crc32_in)

#define BOOTLOADER_flush_app1(addr, buffer, length)             BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION1_ADDRESS, IAP_APPLICATION_LENGTH)
#define BOOTLOADER_flush_app1_settings(addr, buffer, length)    BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION1_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
#define BOOTLOADER_erase_app1(addr)                             BOOTLOADER_erase(addr + IAP_APPLICATION1_ADDRESS, IAP_APPLICATION_LENGTH)
#define BOOTLOADER_erase_app1_settings()                        BOOTLOADER_erase(IAP_APPLICATION1_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
#define BOOTLOADER_add_crc32_app1(addr, length, crc32_in)       BOOTLOADER_add_crc32(addr + IAP_APPLICATION1_ADDRESS, length, crc32_in)


#define BOOTLOADER_flush_app2(addr, buffer, length)             BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION2_ADDRESS, IAP_APPLICATION_LENGTH)
#define BOOTLOADER_flush_app2_settings(addr, buffer, length)    BOOTLOADER_flush(addr, buffer, length, IAP_APPLICATION2_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
#define BOOTLOADER_erase_app2(addr)                             BOOTLOADER_erase(addr + IAP_APPLICATION2_ADDRESS, IAP_APPLICATION_LENGTH)
#define BOOTLOADER_erase_app2_settings()                        BOOTLOADER_erase(IAP_APPLICATION2_SETTINGS_ADDRESS, IAP_APPLICATION_SETTINGS_LENGTH)
#define BOOTLOADER_add_crc32_app2(addr, length, crc32_in)       BOOTLOADER_add_crc32(addr + IAP_APPLICATION2_ADDRESS, length, crc32_in)



    int IAP_run_app1(void);
    int IAP_run(void);
    int IAP_load_settings(void);
    void IAP_stop(void);

#ifdef __cplusplus
}
#endif

#endif
