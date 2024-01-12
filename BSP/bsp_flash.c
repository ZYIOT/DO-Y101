
#include "utils.h"
#include "app.h"
#include "bsp_flash.h"

static void _init(void)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    HAL_FLASH_Lock();
}

int BSP_FLASH_erase(uint32_t address, uint32_t length)
{
    _init();
    uint32_t PageError = 0;
    FLASH_EraseInitTypeDef pEraseInit;
    HAL_StatusTypeDef status = HAL_OK;
    HAL_FLASH_Unlock();
    pEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    pEraseInit.PageAddress = address;
    pEraseInit.Banks = FLASH_BANK_1;
    pEraseInit.NbPages = (length+FLASH_PAGE_SIZE-1) / FLASH_PAGE_SIZE;
    status = HAL_FLASHEx_Erase(&pEraseInit, &PageError);
    HAL_FLASH_Lock();
    if (status != HAL_OK)
    {
        return APP_ERROR;
    }
    return APP_OK;
}

int BSP_FLASH_flush(uint32_t addr, uint32_t *buffer, size_t length, uint32_t start_addr, uint32_t max_length)
{
    uint32_t i = 0;
    HAL_FLASH_Unlock();
    uint32_t destination = start_addr + addr;
    length /= 4;
    for (i = 0; (i < length) && (addr <= (start_addr + max_length - 4)); i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, destination, *(uint32_t *)(buffer + i)) == HAL_OK)
        {
            if (*(uint32_t *)destination != *(uint32_t *)(buffer + i))
            {
                HAL_FLASH_Lock();
                return APP_ERROR;
            }
            destination += 4;
        }
        else
        {
            HAL_FLASH_Lock();
            return APP_ERROR;
        }
    }
    HAL_FLASH_Lock();
    return APP_OK;
}

uint32_t BSP_FLASH_crc32(uint32_t address, uint32_t length)
{
    return crc32((uint8_t *)address, length);
}


void BSP_FLASH_read(uint32_t addr, uint32_t *buffer, uint32_t length)
{
    uint32_t i = 0;
    for(i=0; i<length; i++)
	{
		buffer[i] = *(__IO uint32_t *)addr;//读取4个字节.
		addr += 4;//偏移4个字节.	
	}
}

#define UID_BASE	(0x1FFFF7E8)
void BSP_FLASH_get_uid(uint32_t *uid)
{
    uint32_t uid_addr = 0;
    uid_addr = (UID_BASE >> 24) & 0xFF;
    uid_addr <<= 8;
    uid_addr |= (UID_BASE >> 16) & 0xFF;
    uid_addr <<= 8;
    uid_addr |= (UID_BASE >> 8) & 0xFF;
    uid_addr <<= 8;
    uid_addr |= (UID_BASE >> 0) & 0xFF;
	uid[0] = *((__IO uint32_t *)(uid_addr));
	uid[1] = *((__IO uint32_t *)(uid_addr + 4));
	uid[2] = *((__IO uint32_t *)(uid_addr + 8));	
}

#define FLASH_SIZE_BASE		(FLASH_SIZE_DATA_REGISTER)
uint16_t BSP_FLASH_get_flash_size(void)
{
	uint16_t flash_size = 0;
	flash_size = *((__IO uint32_t *)(FLASH_SIZE_BASE)) & 0xFFFF;
	return flash_size;
}
