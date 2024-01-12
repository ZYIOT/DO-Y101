#include "string.h"
#include "main.h"
#include "usart.h"
#include "bsp_usart.h"
typedef int (*uart_process_message_fun)(uint8_t *buffer, size_t len);

uart_buffer_t uart1_rx_buffer;
StreamBufferHandle_t uart1_recv_buffer = NULL;

static int _send_to_uart1_recv_buffer(uint8_t *buffer, size_t len)
{
  if (len > 0)
  {
    xStreamBufferSendFromISR(uart1_recv_buffer, buffer, len, NULL);
  }
  return 0;
}

static int _process_receive_buffer(UART_HandleTypeDef *huart, uart_buffer_t *buffer, uart_process_message_fun callback, int dma_tc)
{
  #if 0
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
  #else
  
	volatile size_t length = 0;
	volatile size_t pos = 0;
	size_t dmacnt = __HAL_DMA_GET_COUNTER(huart->hdmarx);

	pos = UART_BUFFER_SIZE - dmacnt;
	if (dma_tc)
	{
		length = UART_BUFFER_SIZE - buffer->pos;
		callback(&buffer->buffer[buffer->pos], length);
	}
	else
	{
		if (pos > 0)
		{
			length = pos - buffer->pos;
			callback(&buffer->buffer[buffer->pos], length);
		}
	}
	buffer->pos = pos & UART_BUFFER_SIZE_MASK;
	return 0;
  #endif
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
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);
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

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
    MX_USART1_UART_Init();
    uart1_rx_buffer.pos = 0;
    _usart_dma_init(&huart1, &uart1_rx_buffer, NULL);
  }
}

void BSP_UART_init(void)
{
  uart1_recv_buffer = xStreamBufferCreate(UART_STREAMBUF_SIZE, 1);
  uart1_rx_buffer.pos = 0;
  _usart_dma_init(&huart1, &uart1_rx_buffer, NULL);
}
