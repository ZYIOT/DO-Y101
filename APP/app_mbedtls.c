#include "app_mbedtls.h"
#include "bsp_flash.h"
#include "bsp_log.h"
#include "bootloader.h"
#include "iap.h"
#include "md5.h"
#include <stdlib.h>
#include "utils.h"

#define SHOW_MBEDTLS_MESSAGE_DEBUG      // 调试时查看mbedtls信息和RDP信息时定义的宏 
#ifdef SHOW_MBEDTLS_MESSAGE_DEBUG
    #define APP_MBEDTLS_SHOW        BSP_LOG_debug
    #define APP_MBEDTLS_SHOW_HEX    BSP_LOG_debug_hex
#else
    #define APP_MBEDTLS_SHOW        BSP_LOG_trace
    #define APP_MBEDTLS_SHOW_HEX    BSP_LOG_trace_hex
#endif

#define KEY_STORE_LENGTH        (5)
#define CONTENT_LENGTH          (32)
#define PASSWORD_LENGTH         (16)
#define MBEDTLS_MD5_BASEADDR    (APPLICATION_USER_DATA_ADDRESS + FLASH_PAGE_SIZE)
#define MD5_CONTENT_OFFSET      (0)
#define MD5_PASSWORD_OFFSET     (MD5_CONTENT_OFFSET + CONTENT_LENGTH)
#define MD5_RESERVED_OFFSET     (MD5_PASSWORD_OFFSET + PASSWORD_LENGTH)
typedef struct 
{
    uint8_t content[CONTENT_LENGTH];
    uint32_t key[KEY_STORE_LENGTH];
    uint8_t password[PASSWORD_LENGTH];
}app_mbedtls_t, *app_mbedtls_pt;

static app_mbedtls_t md5_mbedtls;



void _gen_md5_password(uint8_t *password)
{
    usrHmacMd5(md5_mbedtls.content, strlen((const char *)(md5_mbedtls.content)), (uint8_t *)(md5_mbedtls.key), password);
    
    APP_MBEDTLS_SHOW("content:[\r\n");
    APP_MBEDTLS_SHOW_HEX((md5_mbedtls.content), strlen((const char *)(md5_mbedtls.content)));
    APP_MBEDTLS_SHOW("\r\n]\r\n");

    APP_MBEDTLS_SHOW("key:[\r\n");
    APP_MBEDTLS_SHOW_HEX((uint8_t *)(md5_mbedtls.key), KEY_STORE_LENGTH * sizeof(uint32_t));
    APP_MBEDTLS_SHOW("\r\n]\r\n");

    APP_MBEDTLS_SHOW("gen password:[\r\n");
    APP_MBEDTLS_SHOW_HEX(password, PASSWORD_LENGTH);
    APP_MBEDTLS_SHOW("\r\n]\r\n");
}

int _load_mbedtls(app_mbedtls_pt mbedtls)
{
    int rc = APP_OK;
    memset(mbedtls->content, 0, CONTENT_LENGTH);
    BSP_FLASH_read(MBEDTLS_MD5_BASEADDR + MD5_CONTENT_OFFSET, (uint32_t *)(mbedtls->content), CONTENT_LENGTH);
    memset(mbedtls->password, 0, PASSWORD_LENGTH);
    BSP_FLASH_read(MBEDTLS_MD5_BASEADDR + MD5_PASSWORD_OFFSET, (uint32_t *)(mbedtls->password), PASSWORD_LENGTH);
    memset((uint8_t *)(mbedtls->key), 0, KEY_STORE_LENGTH * sizeof(uint32_t));
    BSP_FLASH_get_uid(mbedtls->key);
    mbedtls->key[3] = crc32((uint8_t *)(mbedtls->key), 3 * sizeof(uint32_t));   // 在 uid 后增加 4字节的 crc32 ,加大破解难度 
    APP_MBEDTLS_SHOW("load password:[\r\n");
    APP_MBEDTLS_SHOW_HEX(mbedtls->password, PASSWORD_LENGTH);
    APP_MBEDTLS_SHOW("\r\n]\r\n");
    return rc;
}


int _save_mbedtls(app_mbedtls_pt mbedtls)
{
    int rc = APP_OK;
    BSP_FLASH_erase(MBEDTLS_MD5_BASEADDR, FLASH_PAGE_SIZE);
    BSP_FLASH_flush(MD5_CONTENT_OFFSET, (uint32_t *)(mbedtls->content), CONTENT_LENGTH, MBEDTLS_MD5_BASEADDR, CONTENT_LENGTH);
    BSP_FLASH_flush(MD5_PASSWORD_OFFSET, (uint32_t *)(mbedtls->password), PASSWORD_LENGTH, MBEDTLS_MD5_BASEADDR, PASSWORD_LENGTH);
    // 填充其他有效数据 
    return rc;
}

int _judge_pwd(uint8_t *password, uint8_t *gen_password, uint16_t len)
{
    int rc = APP_OK;
    rc = memcmp(password, gen_password, len) ? APP_ERROR : APP_OK;
    return rc;
}

void APP_Flash_RDP_init(uint8_t status)
{
    int rc = APP_OK;
    if(1 == status)
    {
        rc = BOOTLOADER_set_read_protect();
        if(APP_OK != rc)
        {
            BSP_LOG_error("set flash RDP fail\r\n");
        }
        else
        {
            APP_MBEDTLS_SHOW("flash RDP enable\r\n");
        }
    }
    else
    {
        rc = BOOTLOADER_unset_read_protect();
        if(APP_OK != rc)
        {
            BSP_LOG_error("unset flash RDP fail\r\n");
        }
        else
        {
            APP_MBEDTLS_SHOW("flash RDP disable\r\n");
        }
    }    
}



void APP_MBEDTLS_task_run(void *argument)
{
    uint8_t pwd[16] = {0};
    static uint8_t check = 1;
    for (;;)
    {
        HARDWARE_OS_DELAY_MS(5*60*1000);
        BSP_WDG_feed();
        if(1 == check)
        {
            _load_mbedtls(&md5_mbedtls);
            _gen_md5_password(pwd);
            if(APP_OK != _judge_pwd(md5_mbedtls.password, pwd, sizeof(pwd)))
            {
                BSP_LOG_error("error password\r\n");
                check = 0;
                APP_Flash_RDP_init(0);
            }
        }
    }
}
