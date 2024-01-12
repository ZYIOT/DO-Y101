
#ifndef __XMODEM_H__
#define __XMODEM_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include "stdint.h"

#define DLY_1S 1200
#define XMODEM_MAXRETRANS 30
#define XMODEM_BUFFER_SIZE 1050

#include "xmodem_wrapper.h"
    typedef struct
    {
        uint8_t buffer[XMODEM_BUFFER_SIZE];
        uint32_t buffer_size;
        uint8_t header;
        uint8_t current_packet_number;
        uint8_t packet_number1;
        uint8_t packet_number2;
        uint16_t crc;
        uint8_t error_count;
        uint32_t total_size;
        uint8_t last_status; //last process status
        uint8_t success;
    } xmodem_data_t;

    int XMODEM_init(void);
    int XMODEM_waitfor_upload(void);
    int XMODEM_upload(void);
    int XMODEM_receive(void);
#ifdef __cplusplus
}
#endif
#endif
