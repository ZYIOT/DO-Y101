#include "app.h"
#include "usart.h"
#ifdef ENABLE_TERMINAL
#include <stdio.h>
#include <stdarg.h>
#include "bsp_log.h"
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

int BSP_LOG_init(void)
{
    return APP_OK;
}

void BSP_LOG_print(uint8_t level, const char *fmt, ...)
{
    if (level > BSP_LOG_DEFAULT_LEVEL)
    {
        return;
    }
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void BSP_LOG_print_hex(uint8_t level, uint8_t *buf, size_t len)
{
    if (level > BSP_LOG_DEFAULT_LEVEL)
    {
        return;
    }
    for (int i = 0; i < len; i++)
    {
        printf("%02x ", buf[i]);
    }
}

#endif
