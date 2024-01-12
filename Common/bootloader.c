
#include "utils.h"
#include "app.h"
#include "bootloader.h"
#include "main.h"
#include "iap.h"

static void _init(void)
{
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    HAL_FLASH_Lock();
}

int BOOTLOADER_erase(uint32_t address, uint32_t length)
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

int BOOTLOADER_flush(uint32_t addr, uint32_t *buffer, size_t length, uint32_t start_addr, uint32_t max_length)
{
    uint32_t i = 0;
    HAL_FLASH_Unlock();
    uint32_t destination = start_addr + addr;
    length /= 4;
    for (i = 0; (i < length) && (destination <= (start_addr + addr + max_length - 4)); i++)
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

uint32_t BOOTLOADER_crc32(uint32_t address, uint32_t length)
{
    return crc32((uint8_t *)address, length);
}

uint32_t BOOTLOADER_add_crc32(uint32_t address, uint32_t length, uint32_t crc_in)
{
    return add_crc32((uint8_t *)address, length, crc_in^0xFFFFFFFF);
}

int BOOTLOADER_veryfiy_app2(uint32_t size, uint32_t crc_check)
{
    uint8_t buffer[32] = {0};
    uint32_t crc = crc32((uint8_t *)IAP_APPLICATION2_ADDRESS, size);
    if (crc != crc_check)
    {
        return APP_ERROR;
    }
    write_uint32_t(size, &buffer[0]);
    write_uint32_t(crc_check, &buffer[4]);
    uint32_t settings_crc = crc32(buffer, 28);
    write_uint32_t(settings_crc, &buffer[28]);
    int rc = BOOTLOADER_erase_app2_settings();
    APP_CHECK_RC
    return BOOTLOADER_flush_app2_settings(0, (uint32_t *)buffer, 32);
}



int BOOTLOADER_set_read_protect(void)
{
    int rc = APP_OK;
    static FLASH_OBProgramInitTypeDef opt = {0};
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();
    HAL_FLASHEx_OBGetConfig(&opt);

    if(opt.RDPLevel == OB_RDP_LEVEL0)
    {
        opt.OptionType = OPTIONBYTE_RDP;
        opt.RDPLevel = OB_RDP_LEVEL1;
        if(HAL_OK != HAL_FLASHEx_OBProgram(&opt))
        {
            rc = APP_ERROR;
        }
        else
        {
            HAL_FLASH_OB_Launch();
        }
    }

    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
    return rc;
}



int BOOTLOADER_unset_read_protect(void)
{
    int rc = APP_OK;
    static FLASH_OBProgramInitTypeDef opt = {0};
    HAL_FLASHEx_OBGetConfig(&opt);
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();

    if(opt.RDPLevel == OB_RDP_LEVEL1)
    {
        opt.OptionType = OPTIONBYTE_RDP;
        opt.RDPLevel = OB_RDP_LEVEL0;
        if(HAL_OK != HAL_FLASHEx_OBProgram(&opt))
        {
            rc = APP_ERROR;
        }
    }

    HAL_FLASH_OB_Lock();
    HAL_FLASH_Lock();
    return rc;
}


void BOOTLOADER_read(uint32_t addr, uint32_t *buffer, uint32_t length)
{
    uint32_t i = 0;
    for(i=0; i<length; i++)
	{
		buffer[i] = *(__IO uint32_t *)addr;//读取4个字节.
		addr += 4;//偏移4个字节.	
	}
}

#define UID_BASE	(0x1FFFF7E8)
void BOOTLOADER_get_uid(uint32_t *uid)
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
uint16_t BOOTLOADER_get_flash_size(void)
{
	uint16_t flash_size = 0;
	flash_size = *((__IO uint32_t *)(FLASH_SIZE_BASE)) & 0xFFFF;
	return flash_size;
}
