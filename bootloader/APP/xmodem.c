
#include "bsp_xmodem.h"
#include "bsp_usart.h"
#include "bsp_log.h"
#include "bsp_wdg.h"
#include "xmodem.h"
#include "utils.h"
#include "xmodem_wrapper.h"

#define SOH 0x01
#define STX 0x02
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18
#define CTRLZ 0x1A
#define ABORT 0x08
static uint8_t byte_nak = NAK;
static uint8_t byte_ack = ACK;
static uint8_t byte_can = CAN;
static uint8_t byte_abort = ABORT;
static uint32_t file_crc = 0xFFFFFFFF;

#define XMODEM_FAILURE 101
#define XMODEM_FINISHED 102

#define XMODEM_WRITE_BYTE_NAK() BSP_XMODEM_write(&(byte_nak), 1, 1000)
#define XMODEM_WRITE_BYTE_CAN() BSP_XMODEM_write(&(byte_can), 1, 1000)
#define XMODEM_WRITE_BYTE_ACK() BSP_XMODEM_write(&(byte_ack), 1, 1000)
#define XMODEM_WRITE_BYTE_ABORT() BSP_XMODEM_write(&(byte_abort), 1, 1000)

#define XMODEM_CHECK_RC                             \
    if (rc != APP_OK)                               \
    {                                               \
        rc = xmodem_error_handler(&xmodem_data, 1); \
        APP_CHECK_RC                                \
    }

// * 接收方要求发送方以校验和方式发送时以NAK来请求
// * 接收方要求发送方以CRC校验方式发送时以'C'来请求

static xmodem_data_t xmodem_data;
static void _reset(xmodem_data_t *xmodem_data)
{
    xmodem_data->header = 0;
    xmodem_data->buffer_size = 0;
    xmodem_data->current_packet_number = 1;
    xmodem_data->packet_number1 = 0;
    xmodem_data->packet_number2 = 0;
    xmodem_data->error_count = 0;
    xmodem_data->last_status = 1;
    xmodem_data->success = 1;
}

int XMODEM_init(void)
{
    _reset(&xmodem_data);
    xmodem_data.total_size = 0;
    return APP_OK;
}

int XMODEM_waitfor_upload()
{
    uint32_t tickstart = HARDWARE_GET_TICK();
    int rc = 0;
    while ((HARDWARE_GET_TICK() - tickstart) < XMODEM_WAITFOR_UPLOAD)
    {
        BSP_WDG_feed();
        BSP_XMODEM_receive_clear();
        HARDWARE_HAL_DELAY_MS(200);
        rc = BSP_XMODEM_find("UPLOAD_APP");
        if (rc == APP_OK)
        {
            return APP_OK;
        }
    }
    return APP_ERROR;
}

int XMODEM_upload()
{
    int rc = 0;
    file_crc = 0xFFFFFFFF;
    for (;;)
    {
        BSP_XMODEM_receive_clear();
        if (xmodem_data.last_status == 1)
        {
            XMODEM_WRITE_BYTE_NAK();
        }
        else
        {
            XMODEM_WRITE_BYTE_ACK();
        }
        rc = XMODEM_receive();
        
        if (rc == XMODEM_FINISHED)
        {
            return APP_OK;
        }
        if (rc == XMODEM_FAILURE)
        {
            XMODEM_WRITE_BYTE_CAN();
            XMODEM_WRITE_BYTE_CAN();
            XMODEM_WRITE_BYTE_CAN();
            XMODEM_WRITE_BYTE_CAN();
            XMODEM_WRITE_BYTE_CAN();
            XMODEM_WRITE_BYTE_ABORT();
            XMODEM_WRITE_BYTE_ABORT();
            XMODEM_WRITE_BYTE_ABORT();
            XMODEM_WRITE_BYTE_ABORT();
            XMODEM_WRITE_BYTE_ABORT();
            return APP_ERROR;
        }
    }
}

static int xmodem_error_handler(xmodem_data_t *xmodem_data, uint8_t send_nak)
{
    xmodem_data->error_count++;
    if (xmodem_data->error_count >= XMODEM_MAXRETRANS)
    {
        return XMODEM_FAILURE;
    }
    else
    {
        if (send_nak)
        {
            xmodem_data->last_status = 1;
        }
        return APP_OK;
    }
}

uint8_t checksum8(uint8_t *bytes, size_t len)
{
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++)
    {
        sum += bytes[i];
    }
    return sum;
}

int _receive_and_flush(xmodem_data_t *xmodem_data)
{
    uint8_t crc_buffer[2] = {0};
    if (1 != BSP_XMODEM_read(&(xmodem_data->packet_number1), 1, DLY_1S))
        return APP_ERROR;
    if (1 != BSP_XMODEM_read(&(xmodem_data->packet_number2), 1, DLY_1S))
        return APP_ERROR;
    // BSP_LOG_debug("r1 %d %d %d\r\n", xmodem_data->packet_number1, xmodem_data->packet_number2, xmodem_data->current_packet_number);
    if (((xmodem_data->packet_number1 ^ xmodem_data->packet_number2) != 0xff) ||
        (xmodem_data->current_packet_number != xmodem_data->packet_number1))
        return APP_ERROR;
    int rc = BSP_XMODEM_read(xmodem_data->buffer, xmodem_data->buffer_size, DLY_1S);
    if (xmodem_data->buffer_size != rc)
        return APP_ERROR;

    if (1 != BSP_XMODEM_read(crc_buffer, 1, DLY_1S))
        return APP_ERROR;
    uint8_t chksum = checksum8(xmodem_data->buffer, xmodem_data->buffer_size);
    if (chksum != crc_buffer[0])
    {
        return APP_ERROR;
    }
    // BSP_WDG_feed();
    if (xmodem_data->total_size == 0)
    {
        BSP_XMODEM_app2_erase(0);
    }
    else if((xmodem_data->total_size <= BSP_XMODEM_SEGMENT1_LENGTH) 
    && (xmodem_data->total_size + xmodem_data->buffer_size > BSP_XMODEM_SEGMENT1_LENGTH))
    {
        BSP_XMODEM_app2_erase(BSP_XMODEM_SEGMENT1_LENGTH);
    }
    else if((xmodem_data->total_size <= BSP_XMODEM_SEGMENT2_LENGTH) 
    && (xmodem_data->total_size + xmodem_data->buffer_size > BSP_XMODEM_SEGMENT2_LENGTH))
    {
        BSP_XMODEM_app2_erase(BSP_XMODEM_SEGMENT2_LENGTH);
    }
    else if((xmodem_data->total_size <= BSP_XMODEM_SEGMENT3_LENGTH) 
    && (xmodem_data->total_size + xmodem_data->buffer_size > BSP_XMODEM_SEGMENT3_LENGTH))
    {
        BSP_XMODEM_app2_erase(BSP_XMODEM_SEGMENT3_LENGTH);
    }
    else if((xmodem_data->total_size <= BSP_XMODEM_SEGMENT4_LENGTH) 
    && (xmodem_data->total_size + xmodem_data->buffer_size > BSP_XMODEM_SEGMENT4_LENGTH))
    {
        BSP_XMODEM_app2_erase(BSP_XMODEM_SEGMENT4_LENGTH);
    }
    else if((xmodem_data->total_size <= BSP_XMODEM_SEGMENT5_LENGTH) 
    && (xmodem_data->total_size + xmodem_data->buffer_size > BSP_XMODEM_SEGMENT5_LENGTH))
    {
        BSP_XMODEM_app2_erase(BSP_XMODEM_SEGMENT5_LENGTH);
    }
    else if(xmodem_data->total_size + xmodem_data->buffer_size > BSP_XMODEM_SEGMENT6_LENGTH)
    {
        return APP_ERROR;
    }
    BSP_WDG_feed();
    rc = BSP_XMODEM_app2_flush(xmodem_data->total_size, (uint32_t *)xmodem_data->buffer, xmodem_data->buffer_size);
    APP_CHECK_RC
    if(0xFFFFFFFF == file_crc)
    {
        file_crc = BSP_XMODEM_app2_crc(0, xmodem_data->buffer_size, 0);
    }
    else
    {
        file_crc = BSP_XMODEM_app2_crc(xmodem_data->total_size, xmodem_data->buffer_size, file_crc);
    }
    xmodem_data->total_size += xmodem_data->buffer_size;
    xmodem_data->current_packet_number++;
    xmodem_data->last_status = 0;
    xmodem_data->error_count = 0;
    BSP_LOG_trace("total = %04X, size = %04X, crc = %08X\r\n", xmodem_data->total_size, xmodem_data->buffer_size, file_crc);
    return APP_OK;
}

int XMODEM_receive()
{
    int rc = 0;
    BSP_WDG_feed();
    if (1 == BSP_XMODEM_read(&(xmodem_data.header), 1, DLY_1S))
    {
        BSP_WDG_feed();
        switch (xmodem_data.header)
        {
        case SOH:
            xmodem_data.buffer_size = 128;
            rc = _receive_and_flush(&xmodem_data);
            // BSP_LOG_debug("SOH: %d\r\n", rc);
            XMODEM_CHECK_RC
            break;
        case STX:
            xmodem_data.buffer_size = 1024;
            rc = _receive_and_flush(&xmodem_data);
            // BSP_LOG_debug("STX: %d\r\n", rc);
            XMODEM_CHECK_RC
            break;
        case EOT:
            XMODEM_WRITE_BYTE_ACK();
            xmodem_data.success = 0;
            rc = BSP_XMODEM_app2_flush_setting(xmodem_data.total_size, file_crc);
            if(0 != rc)
            {
                BSP_LOG_debug("app2_settings flush fail\r\n");
            }
            else
            {
                BSP_LOG_debug("app2_settings flush OK\r\n");
            }
            return XMODEM_FINISHED;
        case CAN:
            return XMODEM_FAILURE;
        default:
            rc = XMODEM_FAILURE;
            XMODEM_CHECK_RC
        }
    }
    else
    {
        rc = XMODEM_FAILURE;
        XMODEM_CHECK_RC
    }
    return APP_OK;
}
