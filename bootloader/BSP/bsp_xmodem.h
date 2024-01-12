#ifndef __BSP_XMODEM_H
#define __BSP_XMODEM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "iap.h"

// #define BSP_XMODEM_erase() BOOTLOADER_erase_app1()
// #define BSP_XMODEM_flush(addr, buffer, length) BOOTLOADER_flush_app1(addr, buffer, length)
// #define BSP_XMODEM_erase()                      BOOTLOADER_erase_app2()
// #define BSP_XMODEM_erase_ex(addr)               BOOTLOADER_erase_app2_ex(addr)
// #define BSP_XMODEM_flush(addr, buffer, length)  BOOTLOADER_flush_app2(addr, buffer, length)
// #define BSP_XMODEM_flush_setting(size, crc)     BOOTLOADER_veryfiy_app2(size, crc)
// #define BSP_XMODEM_crc(size)                    BOOTLOADER_crc32_app2(size)
// #define BSP_XMODEM_add_crc(addr, size, crc_in)  BOOTLOADER_add_crc32_app2(addr, size, crc_in)
#define BSP_XMODEM_SEGMENT1_LENGTH              (IAP_APPLICATION_LENGTH)
#define BSP_XMODEM_SEGMENT2_LENGTH              (2*IAP_APPLICATION_LENGTH)
#define BSP_XMODEM_SEGMENT3_LENGTH              (3*IAP_APPLICATION_LENGTH)
#define BSP_XMODEM_SEGMENT4_LENGTH              (4*IAP_APPLICATION_LENGTH)
#define BSP_XMODEM_SEGMENT5_LENGTH              (5*IAP_APPLICATION_LENGTH)
#define BSP_XMODEM_SEGMENT6_LENGTH              (IAP_APPLICATION_LENGTH_MAX)

#define BSP_XMODEM_app1_erase(addr)                     BOOTLOADER_erase_app1(addr)
#define BSP_XMODEM_app1_flush(addr, buffer, length)     BOOTLOADER_flush_app1(addr, buffer, length)
#define BSP_XMODEM_app1_flush_setting(size, crc)        BOOTLOADER_veryfiy_app1(size, crc)
#define BSP_XMODEM_app1_crc(addr, size, crc_in)         BOOTLOADER_add_crc32_app1(addr, size, crc_in)

#define BSP_XMODEM_app2_erase(addr)                     BOOTLOADER_erase_app2(addr)
#define BSP_XMODEM_app2_flush(addr, buffer, length)     BOOTLOADER_flush_app2(addr, buffer, length)
#define BSP_XMODEM_app2_flush_setting(size, crc)        BOOTLOADER_veryfiy_app2(size, crc)
#define BSP_XMODEM_app2_crc(addr, size, crc_in)         BOOTLOADER_add_crc32_app2(addr, size, crc_in)

    int BSP_XMODEM_init(void);
    int BSP_XMODEM_receive_clear(void);
    uint16_t BSP_XMODEM_receive_length(void);
    int BSP_XMODEM_write(uint8_t *buffer, size_t len, size_t tick);
    int BSP_XMODEM_read(uint8_t *buffer, size_t len, size_t tick);
    int BSP_XMODEM_find(uint8_t *str);
    int BSP_XMODEM_erase_program(void);

#ifdef __cplusplus
}
#endif

#endif
