#include "modbus_packet.h"
#include "modbus_buffer.h"
static modbus_buffer_t modbus_buffer;

typedef int (*modbus_parser_fun)(modbus_port_pt port, modbus_packet_pt packet);

static void _get_packet_length(modbus_packet_pt packet)
{
  uint16_t len = 0;
  if (packet->exception == 1)
  {
    packet->length = 1;
    return;
  }
  switch (packet->code)
  {
  case MODBUS_MESSAGE_READ_COILS:
  case MODBUS_MESSAGE_READ_DISCRETE_INPUTS:
  case MODBUS_MESSAGE_READ_HOLDING_REGISTERS:
  case MODBUS_MESSAGE_READ_INPUT_REGISTERS:
  case MODBUS_MESSAGE_WRITE_SINGLE_COIL:
  case MODBUS_MESSAGE_WRITE_SINGLE_REGISTER:
  case MODBUS_MESSAGE_DIAGNOSTICS:
    len = 4;
    break;
  case MODBUS_MESSAGE_WRITE_MULTIPLE_COILS:
  case MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS:
    len = 5;
    break;
  case MODBUS_MESSAGE_IAP:
    len = 2 + 256;
    break;
  case MODBUS_MESSAGE_IAP_CHECK:
    len = 8;
    break;
  case MODBUS_MESSAGE_READ_EXCEPTION_STATUS:
  case MODBUS_MESSAGE_GET_COMMON_EVENT_COUNTER:
  case MODBUS_MESSAGE_GET_COMMON_EVENT_LOG:
  case MODBUS_MESSAGE_REPORT_SERVER_ID:
    len = 0;
    break;
  default:
    break;
  }
  packet->length = len;
}

int read_remain_buffer(modbus_buffer_pt modbus_buffer, modbus_packet_pt packet)
{
  if (packet->exception == 1)
    return PROTOCOL_OK;

  int len = 0;
  int pos = 1;
  int rc = 0;
  switch (packet->code)
  {
  case MODBUS_MESSAGE_WRITE_MULTIPLE_COILS:
  case MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS:
    len = packet->buffer[4];
    pos = 5;
    break;
  }
  if (len > 0)
  {
    packet->length += len;
    rc = MODBUS_BUFFER_read(modbus_buffer, &packet->buffer[pos], len);
    MODBUS_CHECK_READ_SIZE_AND_RETURN(len)
  }
  return PROTOCOL_OK;
}

static int _check_header(modbus_packet_pt packet)
{
  uint8_t code = packet->code & 0x7F;
  if ((packet->addr == (MODBUS_PROTOCOL_DEVICE_ID)) &&
      (
          //  code == (MODBUS_MESSAGE_READ_COILS) ||
          //  code == (MODBUS_MESSAGE_READ_DISCRETE_INPUTS) ||
          code == (MODBUS_MESSAGE_READ_HOLDING_REGISTERS) ||
          //  code == (MODBUS_MESSAGE_READ_INPUT_REGISTERS) ||
          //  code == (MODBUS_MESSAGE_WRITE_SINGLE_COIL) ||
          code == (MODBUS_MESSAGE_WRITE_SINGLE_REGISTER) ||
          //  code == (MODBUS_MESSAGE_READ_EXCEPTION_STATUS) ||
          //  code == (MODBUS_MESSAGE_DIAGNOSTICS) ||
          //  code == (MODBUS_MESSAGE_GET_COMMON_EVENT_COUNTER) ||
          //  code == (MODBUS_MESSAGE_GET_COMMON_EVENT_LOG) ||
          //  code == (MODBUS_MESSAGE_WRITE_MULTIPLE_COILS) ||
          code == (MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS)
          //  code == (MODBUS_MESSAGE_REPORT_SERVER_ID) ||
          // code == (MODBUS_MESSAGE_IAP) ||
          //  code == (MODBUS_MESSAGE_IAP_CHECK)
          ))
  {
    _get_packet_length(packet);
    return PROTOCOL_OK;
  }
  return PROTOCOL_ERROR;
}

static uint16_t _header_deserialize(uint8_t *bytes, modbus_packet_pt packet)
{
  uint16_t pos = 0;
  packet->addr = read_uint8_t_BE(&bytes[pos]);
  pos += 1;
  packet->code = read_uint8_t_BE(&bytes[pos]);
  pos += 1;
  packet->exception = ((packet->code & 0x80) == 0x80) ? 1 : 0;
  return pos;
}

static int _find_header(modbus_buffer_pt modbus_buffer, modbus_packet_pt packet)
{
  static uint8_t buffer[MODBUS_HEADER_SIZE];
  int rc = 0;
  // int len = MODBUS_BUFFER_count(modbus_buffer);
  // if (len < MODBUS_HEADER_SIZE)
  // {
  //   return PROTOCOL_READ_ERROR;
  // }
  while (1)
  {
    rc = MODBUS_BUFFER_read(modbus_buffer, buffer, MODBUS_HEADER_SIZE);
    MODBUS_CHECK_READ_SIZE_AND_RETURN(MODBUS_HEADER_SIZE)
    _header_deserialize(buffer, packet);
    if (_check_header(packet) == PROTOCOL_OK)
    {
      return PROTOCOL_OK;
    }
    MODBUS_BUFFER_shift(modbus_buffer);
  }
}

static int _parse_extra(modbus_buffer_pt modbus_buffer, modbus_packet_pt packet)
{
  uint8_t buf[8] = {0};
  int rc = 0;
  if (packet->length > 0)
  {
    rc = MODBUS_BUFFER_read(modbus_buffer, packet->buffer, packet->length);
    MODBUS_CHECK_READ_SIZE_AND_RETURN(packet->length)
    rc = read_remain_buffer(modbus_buffer, packet);
    MODBUS_CHECK_RC
  }
  rc = MODBUS_BUFFER_read(modbus_buffer, buf, 2);
  MODBUS_CHECK_READ_SIZE_AND_RETURN(2)
  packet->crc = read_uint16_t(buf);
  return PROTOCOL_OK;
}

static int _parse_verify_crc(modbus_buffer_pt modbus_buffer, modbus_packet_pt packet)
{
  uint8_t buf[MODBUS_PROTOCOL_BUFFER_SIZE] = {0};
  uint16_t len = MODBUS_PACKET_serialize(packet, buf);
  if (crc16(buf, len - 2) != packet->crc)
  {
    MODBUS_BUFFER_shift(modbus_buffer);
    return PROTOCOL_CRC_ERROR;
  }
  return PROTOCOL_OK;
}

WINAPI_EXPORT int WINAPI MODBUS_init(void)
{
  MODBUS_BUFFER_init(&modbus_buffer);
  return PROTOCOL_OK;
}

WINAPI_EXPORT int WINAPI MODBUS_PACKET_init(modbus_packet_pt packet)
{
  packet->addr = MODBUS_PROTOCOL_DEVICE_ID;
  packet->code = 3;
  memset(packet->buffer, 0, MODBUS_MAX_BUFFER_SIZE);
  packet->crc = 0;
  packet->length = 0;
  packet->exception = 0;
  packet->parsed = NULL;
  return PROTOCOL_OK;
}

WINAPI_EXPORT uint16_t WINAPI MODBUS_PACKET_serialize(modbus_packet_pt packet, uint8_t bytes[MODBUS_PROTOCOL_BUFFER_SIZE])
{
  uint16_t pos = 0;
  write_uint8_t_BE(packet->addr, &bytes[pos]);
  pos += 1;
  write_uint8_t_BE(packet->code, &bytes[pos]);
  pos += 1;
  memcpy(&bytes[pos], packet->buffer, packet->length);
  pos += packet->length;
  write_uint16_t(packet->crc, &bytes[pos]);
  pos += 2;
  return pos;
}

WINAPI_EXPORT int WINAPI MODBUS_PACKET_read(modbus_port_pt port, modbus_packet_pt packet)
{
  int rc = 0;
  modbus_buffer.port = port;
  rc = _find_header(&modbus_buffer, packet);
  MODBUS_CHECK_RC
  rc = _parse_extra(&modbus_buffer, packet);
  MODBUS_CHECK_RC
  rc = _parse_verify_crc(&modbus_buffer, packet);
  MODBUS_CHECK_RC
  MODBUS_BUFFER_reset(&modbus_buffer);
  packet->exception = ((packet->code & 0x80) == 0x80) ? 1 : 0;
  return rc;
}

WINAPI_EXPORT int WINAPI MODBUS_PACKET_write(modbus_port_pt port, modbus_packet_pt packet)
{
  uint8_t buf[MODBUS_PROTOCOL_BUFFER_SIZE] = {0};
  uint16_t len = MODBUS_PACKET_serialize(packet, buf);
  if (len <= 2)
  {
    return PROTOCOL_ERROR;
  }
  uint16_t crc = crc16(buf, len - 2);
  write_uint16_t(crc, &buf[len - 2]);
  port->write(buf, len, MODBUS_WRITE_TIMEOUT);
  return PROTOCOL_OK;
}
