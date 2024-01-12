#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#define ENABLE_TERMINAL
#define ENABLE_LOG_TRACE
// #define DISABLE_WDG
//#define ENABLE_FLASH_RDP        // 使能 flash read protect 功能
// #define ENABLE_MBEDTLS_MD5      // 使能 mbedtls_md5 加密功能 

#define PN_CODE "DO-Y101"
#define SN_CODE 3101

#define APP_FIRMWARE_MAJOR 1
#define APP_FIRMWARE_MINOR 0
#define APP_FIRMWARE_REVISION 0

#define APP_HARDWARE_MAJOR 1
#define APP_HARDWARE_MINOR 0
#define APP_HARDWARE_REVISION 0

#ifdef __cplusplus
}
#endif

#endif
