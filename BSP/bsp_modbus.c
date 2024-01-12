#include "main.h"
#include "FreeRTOS.h"
#include "stream_Buffer.h"
#include "cmsis_os.h"
#include "bsp_usart.h"
#include "bsp_modbus.h"
#include "bsp_log.h"
#include "app.h"
#include "hardware_wrapper.h"


static HARDWARE_SEMAPHORE_TYPE_T sem;
#define SET_RS485_OUT() (HAL_GPIO_WritePin(RS485_WRITE_GPIO_Port, RS485_WRITE_Pin, GPIO_PIN_SET))
#define SET_RS485_IN()  (HAL_GPIO_WritePin(RS485_WRITE_GPIO_Port, RS485_WRITE_Pin, GPIO_PIN_RESET))
#define GET_RS485_DIR() (HAL_GPIO_ReadPin(RS485_WRITE_GPIO_Port, RS485_WRITE_Pin))
#define RS485_OUT       GPIO_PIN_SET
#define RS485_IN        GPIO_PIN_RESET


int BSP_MODBUS_init(void)
{
    sem = HARDWARE_CREATE_SEMAPHORE();
    SET_RS485_IN();
    MODBUS_init();
    return APP_OK;
}



int _write(uint8_t buffer[MODBUS_PROTOCOL_BUFFER_SIZE], size_t len, size_t tick)
{
    #ifdef SHOW_MODBUS_DATA_DEBUG
    BSP_LOG_show("R write:  len=%d, data=[ ", len);
    BSP_LOG_show_hex(buffer, len);
    BSP_LOG_show("]\r\n");
    #endif
    HARDWARE_TAKE_SEMAPHORE(sem);
    SET_RS485_OUT();
    int rc = HAL_UART_Transmit(&huart1, buffer, len, tick) == HAL_OK ? PROTOCOL_OK : PROTOCOL_ERROR;
    SET_RS485_IN();
    HARDWARE_GIVE_SEMAPHORE(sem);
    return rc;
}

int _read(uint8_t buffer[MODBUS_PROTOCOL_BUFFER_SIZE], size_t len, size_t tick)
{
    int index = 0, rc = 0;
    #ifdef SHOW_MODBUS_DATA_DEBUG
    BSP_LOG_show("R read:  len=%d, data=[ ", len);
    #endif
	SET_RS485_IN();
    do
    {
        rc = xStreamBufferReceive(uart1_recv_buffer, &buffer[index], len - index, tick);
        MODBUS_CHECK_READ_RC_AND_RETURN
        #ifdef SHOW_MODBUS_DATA_DEBUG
        BSP_LOG_show_hex(&buffer[index], rc);
        #endif
        index += rc;
    } while (index < len);
    #ifdef SHOW_MODBUS_DATA_DEBUG
    BSP_LOG_show("]\r\n");
    #endif
    return index;
}

void _clear(void)
{
    xStreamBufferReset(uart1_recv_buffer);
}

static size_t _length(void)
{
    return xStreamBufferBytesAvailable(uart1_recv_buffer);
}

modbus_port_t modbus_port = {
    .read = _read,
    .write = _write,
    .clear = _clear,
    .length = _length};

int BSP_MODBUS_send_error_response(modbus_packet_pt packet, uint8_t status)
{
    modbus_error_response_message_t message;
    MODBUS_error_response_message_init(&message);
    message.status = status;
    packet->length = MODBUS_error_response_message_serialize(packet->buffer, &message);
    packet->parsed = &message;
    packet->code |= 0x80;
    MODBUS_PACKET_write(&modbus_port, packet);
    return APP_OK;
}
