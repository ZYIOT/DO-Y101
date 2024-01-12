#ifndef __BSP_USART_H
#define __BSP_USART_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "app.h"
#include "main.h"
#include "usart.h"

// #define UART_BUFFER_SIZE 100
#define UART_BUFFER_SIZE 		(SIZE_128B)
#define UART_BUFFER_SIZE_MASK	(UART_BUFFER_SIZE - 1)
#define UART_STREAMBUF_SIZE		(SIZE_256B)
#define UART_IDEL_TICKS 300

  typedef struct
  {
    uint8_t buffer[UART_BUFFER_SIZE];
    volatile size_t pos;
  } uart_buffer_t;

  extern StreamBufferHandle_t uart1_recv_buffer;

  void BSP_UART_init(void);
  void BSP_UART_DMA_idle_irq_handler(UART_HandleTypeDef *huart);
  void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
  
#ifdef __cplusplus
}
#endif

#endif
