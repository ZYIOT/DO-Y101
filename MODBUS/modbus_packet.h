#ifndef __MODBUS_PACKET_H
#define __MODBUS_PACKET_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "modbus_common.h"

  typedef struct
  {
    uint8_t addr;                           /* �豸��ַ */
    uint8_t code;                           /* ������ */
    uint8_t buffer[MODBUS_MAX_BUFFER_SIZE]; /* ���� */
    uint16_t crc;                           /* CRC */
    uint16_t length;                        /* ���ݳ��� */
    uint8_t exception;                      /* �쳣 */
    void *parsed;
  } modbus_packet_t, *modbus_packet_pt;

  WINAPI_EXPORT int WINAPI MODBUS_init(void);
  WINAPI_EXPORT int WINAPI MODBUS_PACKET_init(modbus_packet_pt packet);
  WINAPI_EXPORT uint16_t WINAPI MODBUS_PACKET_serialize(modbus_packet_pt packet, uint8_t bytes[MODBUS_PROTOCOL_BUFFER_SIZE]);
  WINAPI_EXPORT int WINAPI MODBUS_PACKET_read(modbus_port_pt port, modbus_packet_pt packet);
  WINAPI_EXPORT int WINAPI MODBUS_PACKET_write(modbus_port_pt port, modbus_packet_pt packet);

#ifdef __cplusplus
}
#endif
#endif
