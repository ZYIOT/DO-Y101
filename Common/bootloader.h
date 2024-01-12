#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "app.h"

    int BOOTLOADER_erase(uint32_t address, uint32_t length);
    int BOOTLOADER_flush(uint32_t addr, uint32_t *buffer, size_t length, uint32_t start_addr, uint32_t max_length);
    void BOOTLOADER_read(uint32_t addr, uint32_t *buffer, uint32_t length);
    uint32_t BOOTLOADER_crc32(uint32_t address, uint32_t length);
    uint32_t BOOTLOADER_add_crc32(uint32_t address, uint32_t length, uint32_t crc_in);
    int BOOTLOADER_veryfiy_app2(uint32_t size, uint32_t crc_check);

    int BOOTLOADER_set_read_protect(void);
    int BOOTLOADER_unset_read_protect(void);
    void BOOTLOADER_get_uid(uint32_t *uid);
    uint16_t BOOTLOADER_get_flash_size(void);
#ifdef __cplusplus
}
#endif

#endif
