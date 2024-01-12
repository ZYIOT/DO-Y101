#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

int BSP_FLASH_erase(uint32_t address, uint32_t length);
int BSP_FLASH_flush(uint32_t addr, uint32_t *buffer, size_t length, uint32_t start_addr, uint32_t max_length);
uint32_t BSP_FLASH_crc32(uint32_t address, uint32_t length);
void BSP_FLASH_read(uint32_t addr, uint32_t *buffer, uint32_t length);
void BSP_FLASH_get_uid(uint32_t *uid);
uint16_t BSP_FLASH_get_flash_size(void);

#ifdef __cplusplus
}
#endif

#endif
