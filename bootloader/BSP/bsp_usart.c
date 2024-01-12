#include "string.h"
#include "main.h"
#include "usart.h"
#include "bsp_usart.h"
#include "bsp_log.h"
typedef int (*uart_process_message_fun)(uint8_t *buffer, size_t len);

#define UART_RING_BUFFER_SIZE 20000
uart_buffer_t uart1_rx_buffer;
uint8_t uart1_received_buffer[UART_RING_BUFFER_SIZE];
ring_buffer_t uart1_recv_ring_buffer;

static int _send_to_uart1_recv_buffer(uint8_t *buffer, size_t len)
{
  if (len > 0)
  {
    RingBuffer_write(&uart1_recv_ring_buffer, buffer, len);
  }
  return 0;
}

static int _process_receive_buffer(UART_HandleTypeDef *huart, uart_buffer_t *buffer, uart_process_message_fun callback, int dma_tc)
{
  volatile size_t length = 0;
  volatile size_t pos = UART_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart->hdmarx);
  if (pos != buffer->pos || dma_tc)
  {
    if (pos > buffer->pos)
    {
      length = pos - buffer->pos;
      callback(&buffer->buffer[buffer->pos], length);
    }
    else
    {
      length = UART_BUFFER_SIZE - buffer->pos;
      callback(&buffer->buffer[buffer->pos], length);
      length = pos;
      callback(&buffer->buffer[0], length);
    }
    buffer->pos = pos % UART_BUFFER_SIZE;
  }
  return 0;
}

static void HAL_UART_process_callback(UART_HandleTypeDef *huart, int dma_ct)
{
  if (huart->Instance == USART1)
  {
    _process_receive_buffer(huart, &uart1_rx_buffer, _send_to_uart1_recv_buffer, dma_ct);
  }
}

static void _usart_dma_init(UART_HandleTypeDef *huart, uart_buffer_t *rx, uart_buffer_t *tx)
{
  __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);
  __HAL_UART_DISABLE_IT(huart, UART_IT_LBD);
  __HAL_UART_DISABLE_IT(huart, UART_IT_PE);
  __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);
  __HAL_UART_ENABLE_IT(huart, UART_IT_IDLE);
  if (rx != NULL)
  {
    rx->pos = 0;
    HAL_UART_Receive_DMA(huart, rx->buffer, UART_BUFFER_SIZE);
  }
  if (tx != NULL)
  {
    tx->pos = 0;
    HAL_UART_Transmit_DMA(huart, tx->buffer, UART_BUFFER_SIZE);
  }
  __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
}

void BSP_UART_DMA_idle_irq_handler(UART_HandleTypeDef *huart)
{
  if ((__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET) &&
      (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) == RESET) &&
      (__HAL_UART_GET_FLAG(huart, UART_FLAG_NE) == RESET) &&
      (__HAL_UART_GET_FLAG(huart, UART_FLAG_FE) == RESET) &&
      (__HAL_UART_GET_FLAG(huart, UART_FLAG_PE) == RESET)) // && (__HAL_UART_GET_IT_SOURCE(huart, UART_IT_IDLE) != RESET))
  {
    HAL_UART_process_callback(huart, 0);
  }
  __HAL_UART_CLEAR_IDLEFLAG(huart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_process_callback(huart, 1);
}

void BSP_UART_init(void)
{
  RingBuffer_init(&uart1_recv_ring_buffer, uart1_received_buffer, UART_RING_BUFFER_SIZE);
  _usart_dma_init(&huart1, &uart1_rx_buffer, NULL);
}
