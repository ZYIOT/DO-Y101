#include "bsp_xmodem.h"
#include "usart.h"
#include "bsp_usart.h"
#include "bsp_log.h"
#include "bsp_wdg.h"
#include "iap.h"

#define __XMODEM_CHECK_READ_RC_AND_RETURN \
    if (rc < 0)                           \
    {                                     \
        return APP_ERROR;                 \
    }

int BSP_XMODEM_init(void)
{
    return APP_OK;
}

int BSP_XMODEM_find(uint8_t *str)
{
    uint8_t bytes[32];
    memset(bytes, 0, 32);
    int len = RingBuffer_read(&uart1_recv_ring_buffer, bytes, 30);
    if (len == 0)
    {
        return APP_ERROR;
    }
    char *ret = strstr((const char *)bytes, (const char *)str);
    if (ret == NULL)
    {
        return APP_ERROR;
    }
    return APP_OK;
}

int BSP_XMODEM_read(uint8_t *buffer, size_t len, size_t tick)
{
    int index = 0, rc = 0;
    uint32_t tickstart = HARDWARE_GET_TICK();
    BSP_LOG_trace("\r\nread:  len=%d, data=", len);
    do
    {
        BSP_LOG_debug("\r\n%d\r\n", HARDWARE_GET_TICK());
        rc = RingBuffer_read(&uart1_recv_ring_buffer, buffer, len);
        __XMODEM_CHECK_READ_RC_AND_RETURN
        BSP_LOG_trace_hex(&(buffer[index]), rc);
        index += rc;
        if (index < len)
        {
            HARDWARE_HAL_DELAY_MS(30);
        }
    } while (index < len && (HARDWARE_GET_TICK() - tickstart) < tick);
    BSP_LOG_trace("\r\n");
    BSP_WDG_feed();
    return index;
}

int BSP_XMODEM_write(uint8_t *buffer, size_t len, size_t tick)
{
    HAL_GPIO_WritePin(RS485_WRITE_GPIO_Port, RS485_WRITE_Pin, GPIO_PIN_SET);
    HAL_UART_Transmit(&huart1, buffer, len, tick);
    HAL_GPIO_WritePin(RS485_WRITE_GPIO_Port, RS485_WRITE_Pin, GPIO_PIN_RESET);
    return len;
}

int BSP_XMODEM_receive_clear(void)
{
    RingBuffer_reset(&uart1_recv_ring_buffer);
    return 0;
}

uint16_t BSP_XMODEM_receive_length(void)
{
    return RingBuffer_count(&uart1_recv_ring_buffer);
}

int BSP_XMODEM_erase_program(void)
{
    int rc = APP_OK;
    uint32_t i = 0;
    BSP_WDG_feed();
    while(i <= IAP_APPLICATION_LENGTH_MAX)
    {
        rc = BSP_XMODEM_app1_erase(i);
        APP_CHECK_RC
        i += IAP_APPLICATION_LENGTH;
        BSP_WDG_feed();
    }
    BOOTLOADER_erase_app1_settings();
    i = 0;
    BSP_WDG_feed();
    while(i <= IAP_APPLICATION_LENGTH_MAX)
    {
        rc = BSP_XMODEM_app2_erase(i);
        APP_CHECK_RC
        i += IAP_APPLICATION_LENGTH;
        BSP_WDG_feed();
    }
    BOOTLOADER_erase_app2_settings();
    BSP_WDG_feed();
    return rc;
}

