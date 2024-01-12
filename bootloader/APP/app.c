#include "app.h"
#include "iap.h"
#include "bsp_wdg.h"
#include "bsp_mcu.h"
#include "bsp_usart.h"
#include "bsp_gpio.h"
#include "bsp_log.h"
#include "xmodem.h"
#include "md5.h"

// #define SHOW_MBEDTLS_MESSAGE_DEBUG      // 调试时查看mbedtls信息和RDP信息时定义的宏 
#ifdef SHOW_MBEDTLS_MESSAGE_DEBUG
    #define APP_MBEDTLS_SHOW        BSP_LOG_debug
    #define APP_MBEDTLS_SHOW_HEX    BSP_LOG_debug_hex
#else
    #define APP_MBEDTLS_SHOW        BSP_LOG_trace
    #define APP_MBEDTLS_SHOW_HEX    BSP_LOG_trace_hex
#endif

void APP_Flash_RDP_init(uint8_t status)
{
    int rc = APP_OK;
    if(1 == status)
    {
        rc = BOOTLOADER_set_read_protect();
        if(APP_OK != rc)
        {
            BSP_XMODEM_erase_program();
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
    BOOTLOADER_read(MBEDTLS_MD5_BASEADDR + MD5_CONTENT_OFFSET, (uint32_t *)(mbedtls->content), CONTENT_LENGTH);
    memset(mbedtls->password, 0, PASSWORD_LENGTH);
    BOOTLOADER_read(MBEDTLS_MD5_BASEADDR + MD5_PASSWORD_OFFSET, (uint32_t *)(mbedtls->password), PASSWORD_LENGTH);
    memset((uint8_t *)(mbedtls->key), 0, KEY_STORE_LENGTH * sizeof(uint32_t));
    BOOTLOADER_get_uid(mbedtls->key);
    mbedtls->key[3] = crc32((uint8_t *)(mbedtls->key), 3 * sizeof(uint32_t));   // 在 uid 后增加 4字节的 crc32 ,加大破解难度 
    APP_MBEDTLS_SHOW("load password:[\r\n");
    APP_MBEDTLS_SHOW_HEX(mbedtls->password, PASSWORD_LENGTH);
    APP_MBEDTLS_SHOW("\r\n]\r\n");
    return rc;
}


int _save_mbedtls(app_mbedtls_pt mbedtls)
{
    int rc = APP_OK;
    uint8_t fill_data[32] = {0};
    BOOTLOADER_erase(MBEDTLS_MD5_BASEADDR, FLASH_PAGE_SIZE);
    BOOTLOADER_flush(MD5_CONTENT_OFFSET, (uint32_t *)(mbedtls->content), CONTENT_LENGTH, MBEDTLS_MD5_BASEADDR, CONTENT_LENGTH);
    BOOTLOADER_flush(MD5_PASSWORD_OFFSET, (uint32_t *)(mbedtls->password), PASSWORD_LENGTH, MBEDTLS_MD5_BASEADDR, PASSWORD_LENGTH);
    for(uint8_t i=0; i<32-PASSWORD_LENGTH; i++)
    {
        fill_data[i] = i;
    }
    // 填充其他有效数据,用于干扰分析 
    BOOTLOADER_flush(MD5_RESERVED_OFFSET, (uint32_t *)fill_data, 32-PASSWORD_LENGTH, MBEDTLS_MD5_BASEADDR, 32);
    return rc;
}

int APP_MBEDTLS_md5_check(void)
{
    int rc = APP_OK;
    uint8_t content[CONTENT_LENGTH] = {0};
    uint8_t sn_str[12] = {0};
    _load_mbedtls(&md5_mbedtls);
    strcpy(content, "ZYIOT,");
    strcat(content, PN_CODE",");
    sprintf(sn_str, "%d", SN_CODE);
    strcat(content, sn_str);
    // rc = strcmp(md5_mbedtls.content, content) == 0 ? APP_OK : APP_ERROR;
    rc = BSP_MBEDTLS_CHECK_STATUS();
    if(APP_OK == rc)
    {
        memset(md5_mbedtls.content, 0, CONTENT_LENGTH);
        strcpy(md5_mbedtls.content, content);
        // 填充有效数据到 content 中，干扰分析 
        uint8_t len = CONTENT_LENGTH - strlen(md5_mbedtls.content);
        for(uint8_t i=0; i<len; i++)
        {
            md5_mbedtls.content[CONTENT_LENGTH - len + i] = i;
        }
        memset(md5_mbedtls.password, 0, PASSWORD_LENGTH);
        _gen_md5_password(md5_mbedtls.password);
        _save_mbedtls(&md5_mbedtls);
    }
    return rc;
}

int app_main(void)
{
    BSP_WDG_init();
    BSP_WDG_feed();
    BSP_UART_init();
    BSP_WDG_feed();
    BSP_XMODEM_init();
    XMODEM_init();
    BSP_LOG_debug("BL\r\n");
#ifdef ENABLE_MBEDTLS_MD5    
    APP_MBEDTLS_md5_check();
#endif
#ifdef ENABLE_FLASH_RDP
    APP_Flash_RDP_init(1);
#endif    
    int rc = XMODEM_waitfor_upload();
    if (rc == APP_OK)
    {
        BSP_LOG_debug("upload\r\n");
        XMODEM_upload();
    }
    BSP_LOG_debug("Run\r\n");
    if (IAP_run() != APP_OK)
    {
        BSP_MCU_reset();
    }
    return 0;
}

void IAP_stop(void)
{
    HAL_SuspendTick();
    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL = 0;
    BSP_WDG_stop();
}
