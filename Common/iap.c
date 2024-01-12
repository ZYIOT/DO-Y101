// #include "app.h"
#include "iap.h"
#include "utils.h"
#include "main.h"
#include "bsp_log.h"
#include "bsp_wdg.h"
/*
APP settings的内容
bytes: 0~3 固件的字节数
crc32: 4~7 固件的CRC32
....
crc32: 28~31 配置区的CRC32校验，前28个字节的CRC

bootloader 启动过程
IF(APP2 settings ERROR)
    RUN(APP1)
ELSE
    IF(APP1 settings OK AND APP1 settings == APP2 settings)
        RUN(APP1)
    ELSE
        COPY(APP1, APP2) //把APP2复制到APP1
        COPY(APP1 settings, APP2 settings)//把APP2配置复制到APP1配置
        RUN(APP1)
    END
END

1. APP2 settings的数据不合法，则运行APP1
2. APP1 settings合法， APP2 settings合法， APP1 settings等于APP2 settings，则运行APP1
3. APP2 settings合法，APP1 settings的数据不合法或APP1 settings不等于APP2 settings ，则复制APP2到APP1,并运行APP1
    a. 复制APP2区到APP1区
    b. 校验APP1区的CRC32
    c. 复制APP2 settings到APP1 settings
    d. 校验APP1 settings
    e. 清除APP2 settings
    f. 运行 APP1
注: 
数据是否合法是根据配置的CRC是否正确来判断

APP 更新过程
1. 更新固件到APP2区
2. 计算APP2 settings 并 记录到APP2 settings 区
3. 以前两步都成功后(要校验证数据)， 重启
*/
typedef void (*pFunction)(void);

int IAP_run_app1(void)
{
    BSP_LOG_debug("IAP_run_app1\r\n");
    pFunction JumpToApplication;
    uint32_t JumpAddress;
    if (((*(__IO uint32_t *)IAP_APPLICATION1_ADDRESS) & 0x2FFE0000) == 0x20000000) //检查栈顶地址是否合法.
    {
        __disable_irq(); // 在APP 应用的 SystemInit函数的最后加上 __enable_irq();
        __set_PRIMASK(1); // 关闭STM32总中断
        __set_FAULTMASK(1);
#ifdef USE_HAL_DRIVER
        HAL_RCC_DeInit();
        HAL_DeInit();
#endif
        IAP_stop();
        JumpAddress = *(__IO uint32_t *)(IAP_APPLICATION1_ADDRESS + 4);
        JumpToApplication = (pFunction)JumpAddress;
        __set_PSP(*(__IO uint32_t *)IAP_APPLICATION1_ADDRESS);
        __set_MSP(*(__IO uint32_t *)IAP_APPLICATION1_ADDRESS);
        __set_CONTROL(0);
        JumpToApplication();
    }
    return APP_ERROR;
}

typedef struct
{
    uint32_t bytes;
    uint32_t crc32;
    int rc;
} iap_settings_t, *iap_settings_pt;

iap_settings_t settings1;
iap_settings_t settings2;
static int _load_settings(uint32_t setting_addr, iap_settings_pt settings)
{
    uint8_t buffer[32] = {0};
    memcpy(buffer, (uint8_t *)setting_addr, 32);
    uint32_t crc = read_uint32_t(&buffer[28]);
    uint32_t settings_crc = crc32(buffer, 28);
    if (crc != settings_crc)
    {
        settings->bytes = 0;
        settings->crc32 = 0;
        settings->rc = APP_ERROR;
    }
    else
    {
        settings->bytes = read_uint32_t(buffer);
        settings->crc32 = read_uint32_t(&buffer[4]);
        settings->rc = APP_OK;
    }
    return settings->rc;
}

int IAP_load_settings(void)
{
    int rc = 0;
    rc = _load_settings(IAP_APPLICATION2_SETTINGS_ADDRESS, &settings2);
    APP_CHECK_RC
    return _load_settings(IAP_APPLICATION1_SETTINGS_ADDRESS, &settings1);
}

// int IAP_download_app1(void)
// {
//     BSP_XMODEM_init();
//     BSP_LOG_show("Start program execution......\r\n\n");
//     if (XMODEM_receive() > 10)
//     {
//         IAP_run_app1();
//     }
//     else
//     {
//         BSP_LOG_error("Error: Download image failed\r\n");
//     }
//     return APP_ERROR;
// }

static int _flush_app1_from_app2(void)
{
    BSP_WDG_feed();
    if (settings2.rc != APP_OK)
    {
        return APP_ERROR;
    }
    int rc = BOOTLOADER_erase_app1(0);
    APP_CHECK_RC
    BSP_WDG_feed();
    BSP_LOG_trace("erase app1-0 ok\r\n");
    if(settings2.bytes > IAP_APPLICATION_LENGTH)
    {
        rc = BOOTLOADER_erase_app1(IAP_APPLICATION_LENGTH);
        APP_CHECK_RC
        BSP_WDG_feed();
        BSP_LOG_trace("erase app1-1 ok\r\n");
    }
    if(settings2.bytes > 2*IAP_APPLICATION_LENGTH)
    {
        rc = BOOTLOADER_erase_app1(2*IAP_APPLICATION_LENGTH);
        APP_CHECK_RC
        BSP_WDG_feed();
        BSP_LOG_trace("erase app1-2 ok\r\n");
    }
    if(settings2.bytes > 3*IAP_APPLICATION_LENGTH)
    {
        rc = BOOTLOADER_erase_app1(3*IAP_APPLICATION_LENGTH);
        APP_CHECK_RC
        BSP_WDG_feed();
        BSP_LOG_trace("erase app1-3 ok\r\n");
    }
    if(settings2.bytes > 4*IAP_APPLICATION_LENGTH)
    {
        rc = BOOTLOADER_erase_app1(4*IAP_APPLICATION_LENGTH);
        APP_CHECK_RC
        BSP_WDG_feed();
        BSP_LOG_trace("erase app1-4 ok\r\n");
    }
    if(settings2.bytes > 5*IAP_APPLICATION_LENGTH)
    {
        rc = BOOTLOADER_erase_app1(5*IAP_APPLICATION_LENGTH);
        APP_CHECK_RC
        BSP_WDG_feed();
    }
    BSP_LOG_trace("erase app1 ok\r\n");
    BSP_WDG_feed();
    if(settings2.bytes <= IAP_APPLICATION_LENGTH)
    {
        rc = BOOTLOADER_flush_app1(0, (uint32_t *)IAP_APPLICATION2_ADDRESS, settings2.bytes);
        BSP_LOG_trace("flush app1-0 ok\r\n");
    }
    else
    {
        rc = BOOTLOADER_flush_app1(0, (uint32_t *)IAP_APPLICATION2_ADDRESS, IAP_APPLICATION_LENGTH);
        APP_CHECK_RC
        BSP_WDG_feed();
        BSP_LOG_trace("flush app1-0 ok\r\n");
        if(settings2.bytes <= 2*IAP_APPLICATION_LENGTH)
        {
            rc = BOOTLOADER_flush_app1(IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + IAP_APPLICATION_LENGTH), settings2.bytes - IAP_APPLICATION_LENGTH);
            BSP_LOG_trace("flush app1-1 ok\r\n");
        }
        else
        {
            rc = BOOTLOADER_flush_app1(IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + IAP_APPLICATION_LENGTH), IAP_APPLICATION_LENGTH);
            APP_CHECK_RC
            BSP_WDG_feed();
            BSP_LOG_trace("flush app1-1 ok\r\n");
            if(settings2.bytes <= 3*IAP_APPLICATION_LENGTH)
            {
                rc = BOOTLOADER_flush_app1(2*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 2*IAP_APPLICATION_LENGTH), settings2.bytes - 2*IAP_APPLICATION_LENGTH);
                BSP_LOG_trace("flush app1-2 ok\r\n");
            }
            else
            {
                rc = BOOTLOADER_flush_app1(2*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 2*IAP_APPLICATION_LENGTH), IAP_APPLICATION_LENGTH);
                APP_CHECK_RC
                BSP_WDG_feed();
                BSP_LOG_trace("flush app1-2 ok\r\n");
                if(settings2.bytes <= 4*IAP_APPLICATION_LENGTH)
                {
                    rc = BOOTLOADER_flush_app1(3*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 3*IAP_APPLICATION_LENGTH), settings2.bytes - 3*IAP_APPLICATION_LENGTH);
                    BSP_LOG_trace("flush app1-3 ok\r\n");
                }
                else
                {
                    rc = BOOTLOADER_flush_app1(3*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 3*IAP_APPLICATION_LENGTH), IAP_APPLICATION_LENGTH);
                    APP_CHECK_RC
                    BSP_WDG_feed();
                    BSP_LOG_trace("flush app1-3 ok\r\n");
                    if(settings2.bytes <= 5*IAP_APPLICATION_LENGTH)
                    {
                        rc = BOOTLOADER_flush_app1(4*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 4*IAP_APPLICATION_LENGTH), settings2.bytes - 4*IAP_APPLICATION_LENGTH);
                        BSP_LOG_trace("flush app1-4 ok\r\n");
                    }
                    else
                    {
                        rc = BOOTLOADER_flush_app1(4*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 4*IAP_APPLICATION_LENGTH), IAP_APPLICATION_LENGTH);
                        APP_CHECK_RC
                        BSP_WDG_feed();
                        BSP_LOG_trace("flush app1-4 ok\r\n");
                        rc = BOOTLOADER_flush_app1(5*IAP_APPLICATION_LENGTH, (uint32_t *)(IAP_APPLICATION2_ADDRESS + 5*IAP_APPLICATION_LENGTH), settings2.bytes - 5*IAP_APPLICATION_LENGTH);
                        BSP_LOG_trace("flush app1-5 ok\r\n");
                    }
                }
            }
        }
    }
    APP_CHECK_RC
    BSP_WDG_feed();
    BSP_LOG_trace("flush app1 ok\r\n");
    uint32_t crc = BOOTLOADER_add_crc32_app1(0, settings2.bytes, 0);
    BSP_WDG_feed();
    return (crc == settings2.crc32) ? APP_OK : APP_ERROR;
}

#define SETTINGS_EQUALS()                  \
    (settings1.bytes == settings2.bytes && \
     settings1.crc32 == settings2.crc32 && \
     settings1.rc == APP_OK &&             \
     settings2.rc == APP_OK)

static int _flush_app1_settings_from_app2_settings(void)
{
    if (settings2.rc != APP_OK)
    {
        return APP_ERROR;
    }
    int rc = BOOTLOADER_erase_app1_settings();
    APP_CHECK_RC
    rc = BOOTLOADER_flush_app1_settings(0, (uint32_t *)IAP_APPLICATION2_SETTINGS_ADDRESS, 32);
    APP_CHECK_RC
    rc = _load_settings(IAP_APPLICATION1_SETTINGS_ADDRESS, &settings1);
    APP_CHECK_RC
    return SETTINGS_EQUALS() ? APP_OK : APP_ERROR;
}

int IAP_run(void)
{
    int rc = 0;
    IAP_load_settings();
    if (settings2.rc != APP_OK)
    {
        BSP_LOG_debug("settings2 fail\r\n");
        return IAP_run_app1();
    }
    else
    {
        if (settings1.rc == APP_OK && SETTINGS_EQUALS())
        {
            BSP_LOG_debug("settings1 equals settings2\r\n");
            return IAP_run_app1();
        }
        else
        {
            BSP_LOG_debug("start copy app1 from app2\r\n");
            rc = _flush_app1_from_app2();
            if(APP_OK != rc)
            {
                BSP_LOG_error("crc check fail\r\n");
            }
            else
            {
                BSP_LOG_trace("crc check ok\r\n");
            }
            BSP_LOG_debug("end copy app1 from app2\r\n");
            APP_CHECK_RC
            rc = _flush_app1_settings_from_app2_settings();
            APP_CHECK_RC
            BOOTLOADER_erase_app2_settings();
            return IAP_run_app1();
        }
    }
}

__weak void IAP_stop(void)
{
}
