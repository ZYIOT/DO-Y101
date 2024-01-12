#ifndef __BSP_USART_H
#define __BSP_USART_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stddef.h>
#include <stdint.h>
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "ringbuffer.h"

#define UART_BUFFER_SIZE 10000
#define UART_IDEL_TICKS 300

  extern ring_buffer_t uart1_recv_ring_buffer;

  typedef struct
  {
    uint8_t buffer[UART_BUFFER_SIZE];
    volatile size_t pos;
  } uart_buffer_t;

  void BSP_UART_init(void);
  void BSP_UART_DMA_idle_irq_handler(UART_HandleTypeDef *huart);
  void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#ifdef __cplusplus
}
#endif

#endif
