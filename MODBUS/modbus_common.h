#ifndef __MODBUS_COMMON_H
#define __MODBUS_COMMON_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"

#if defined(WIN32) || defined(_WIN32)
#define WINAPI_EXPORT __declspec(dllexport)
#define __weak
// #ifndef WINAPI
// #define WINAPI __stdcall
// #endif
#define WINAPI
#else
#define WINAPI_EXPORT
#define WINAPI
#endif

#include "app_config.h"

#define PROTOCOL_OK 0
#define PROTOCOL_ERROR -1
#define PROTOCOL_READ_ERROR -2
#define PROTOCOL_CRC_ERROR -3
#define PROTOCOL_NOT_SUPPORT -255
#define MODBUS_READ_TIMEOUT 1000
#define MODBUS_WRITE_TIMEOUT 0x2fff

#define MODBUS_CHECK_RC  \
  if (rc != PROTOCOL_OK) \
  {                      \
    return rc;           \
  }

#define MODBUS_CHECK_READ_RC_AND_RETURN \
  if (rc <= 0)                          \
  {                                     \
    return PROTOCOL_READ_ERROR;         \
  }

#define MODBUS_CHECK_READ_SIZE_AND_RETURN(size) \
  if (rc != size)                               \
  {                                             \
    return PROTOCOL_READ_ERROR;                 \
  }

#define MODBUS_MAX_BUFFER_SIZE 260
#define MODBUS_PROTOCOL_DEVICE_ID APP_CONFIG_device_id()
#define MODBUS_ILLEGAL_FUNCTION 0x01
#define MODBUS_ILLEGAL_DATA_ADDRESS 0x02
#define MODBUS_ILLEGAL_DATA_VALUE 0x03
#define MODBUS_SERVICE_DEVICE_FAILURE 0x04
#define MODBUS_ACKNOWLEDGE 0x05
#define MODBUS_SERVICE_DEVICE_BUSY 0x06
#define MODBUS_MEMORY_PARITY_ERROR 0x08
#define MODBUS_GATEWAY_PATH_UNAVAILABLE 0x0A
#define MODBUS_GATEWAY_TARGET_DEVIE_FAILED_TO_RESPOND 0x0B
#define MODBUS_HEADER_SIZE 2
#define MODBUS_PROTOCOL_BUFFER_SIZE (MODBUS_MAX_BUFFER_SIZE + MODBUS_HEADER_SIZE + 4)

#define MODBUS_MESSAGE_READ_COILS 1
#define MODBUS_MESSAGE_READ_DISCRETE_INPUTS 2
#define MODBUS_MESSAGE_READ_HOLDING_REGISTERS 3
#define MODBUS_MESSAGE_READ_INPUT_REGISTERS 4
#define MODBUS_MESSAGE_WRITE_SINGLE_COIL 5
#define MODBUS_MESSAGE_WRITE_SINGLE_REGISTER 6
#define MODBUS_MESSAGE_READ_EXCEPTION_STATUS 7
#define MODBUS_MESSAGE_DIAGNOSTICS 8
#define MODBUS_MESSAGE_GET_COMMON_EVENT_COUNTER 11
#define MODBUS_MESSAGE_GET_COMMON_EVENT_LOG 12
#define MODBUS_MESSAGE_WRITE_MULTIPLE_COILS 15
#define MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS 16
#define MODBUS_MESSAGE_REPORT_SERVER_ID 17
#define MODBUS_MESSAGE_IAP 0x77
#define MODBUS_MESSAGE_IAP_CHECK 0x78

#define crc16(buf, size) get_modbus_crc16(buf, size)

  typedef int (*modbus_read_fun)(uint8_t buffer[MODBUS_PROTOCOL_BUFFER_SIZE], size_t len, size_t tick);
  typedef int (*modbus_write_fun)(uint8_t buffer[MODBUS_PROTOCOL_BUFFER_SIZE], size_t len, size_t tick);
  typedef void (*modbus_clear_fun)(void);
  typedef size_t (*modbus_length_fun)(void);

  typedef struct
  {
    modbus_read_fun read;
    modbus_write_fun write;
    modbus_clear_fun clear;
    modbus_length_fun length;
  } modbus_port_t, *modbus_port_pt;

#ifdef __cplusplus
}
#endif
#endif
