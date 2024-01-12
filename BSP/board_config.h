#ifndef __BOARD_CONFIG_H
#define __BOARD_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

#define ENABLE_TERMINAL
#define ENABLE_LOG_TRACE
// #define DISABLE_WDG
//#define ENABLE_FLASH_RDP        // ʹ�� flash read protect ����
// #define ENABLE_MBEDTLS_MD5      // ʹ�� mbedtls_md5 ���ܹ��� 

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
