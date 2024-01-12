#include "modbus_slave.h"

static int _on_receive_read_coils_message(modbus_packet_pt packet)
{
  modbus_read_coils_request_message_t message;
  int rc = MODBUS_read_coils_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_read_coils_message_process(packet);
}

static int _on_receive_read_discrete_inputs_message(modbus_packet_pt packet)
{
  modbus_read_discrete_inputs_request_message_t message;
  int rc = MODBUS_read_discrete_inputs_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_read_discrete_inputs_message_process(packet);
}

static int _on_receive_read_holding_registers_message(modbus_packet_pt packet)
{
  modbus_read_holding_registers_request_message_t message;
  int rc = MODBUS_read_holding_registers_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_read_holding_registers_message_process(packet);
}

static int _on_receive_read_input_registers_message(modbus_packet_pt packet)
{
  modbus_read_input_registers_request_message_t message;
  int rc = MODBUS_read_input_registers_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_read_input_registers_message_process(packet);
}

static int _on_receive_write_single_coil_message(modbus_packet_pt packet)
{
  modbus_write_single_coil_message_t message;
  int rc = MODBUS_write_single_coil_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_write_single_coil_message_process(packet);
}

static int _on_receive_write_single_register_message(modbus_packet_pt packet)
{
  modbus_write_single_register_message_t message;
  int rc = MODBUS_write_single_register_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_write_single_register_message_process(packet);
}

static int _on_receive_diagnostics_message(modbus_packet_pt packet)
{
  modbus_diagnostics_message_t message;
  int rc = MODBUS_diagnostics_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_diagnostics_message_process(packet);
}

static int _on_receive_write_multiple_coils_message(modbus_packet_pt packet)
{
  modbus_write_multiple_coils_request_message_t message;
  int rc = MODBUS_write_multiple_coils_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_write_multiple_coils_message_process(packet);
}

static int _on_receive_write_multiple_registers_message(modbus_packet_pt packet)
{
  modbus_write_multiple_registers_request_message_t message;
  int rc = MODBUS_write_multiple_registers_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_write_multiple_registers_message_process(packet);
}

static int _on_receive_IAP_message(modbus_packet_pt packet)
{
  modbus_IAP_request_message_t message;
  int rc = MODBUS_IAP_request_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_IAP_message_process(packet);
}

static int _on_receive_IAP_check_message(modbus_packet_pt packet)
{
  modbus_IAP_check_message_t message;
  int rc = MODBUS_IAP_check_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_IAP_check_message_process(packet);
}

static int _on_receive_error_message(modbus_packet_pt packet)
{
  modbus_error_response_message_t message;
  int rc = MODBUS_error_response_message_deserialize(packet->buffer, &message);
  MODBUS_CHECK_READ_RC_AND_RETURN
  packet->parsed = &message;
  return MODBUS_error_message_process(packet);
}

WINAPI_EXPORT int WINAPI MODBUS_receive_packet(modbus_packet_pt packet)
{
  if (packet->exception == 1)
  {
    return _on_receive_error_message(packet);
  }
  switch (packet->code)
  {
  case MODBUS_MESSAGE_READ_COILS:
    return _on_receive_read_coils_message(packet);
  case MODBUS_MESSAGE_READ_DISCRETE_INPUTS:
    return _on_receive_read_discrete_inputs_message(packet);
  case MODBUS_MESSAGE_READ_HOLDING_REGISTERS:
    return _on_receive_read_holding_registers_message(packet);
  case MODBUS_MESSAGE_READ_INPUT_REGISTERS:
    return _on_receive_read_input_registers_message(packet);
  case MODBUS_MESSAGE_WRITE_SINGLE_COIL:
    return _on_receive_write_single_coil_message(packet);
  case MODBUS_MESSAGE_WRITE_SINGLE_REGISTER:
    return _on_receive_write_single_register_message(packet);
  case MODBUS_MESSAGE_READ_EXCEPTION_STATUS:
    return MODBUS_read_exception_status_message_process(packet);
  case MODBUS_MESSAGE_DIAGNOSTICS:
    return _on_receive_diagnostics_message(packet);
  case MODBUS_MESSAGE_GET_COMMON_EVENT_COUNTER:
    return MODBUS_get_common_event_counter_message_process(packet);
  case MODBUS_MESSAGE_GET_COMMON_EVENT_LOG:
    return MODBUS_get_common_event_log_message_process(packet);
  case MODBUS_MESSAGE_WRITE_MULTIPLE_COILS:
    return _on_receive_write_multiple_coils_message(packet);
  case MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS:
    return _on_receive_write_multiple_registers_message(packet);
  case MODBUS_MESSAGE_REPORT_SERVER_ID:
    return MODBUS_report_server_id_message_process(packet);
  case MODBUS_MESSAGE_IAP:
    return _on_receive_IAP_message(packet);
  case MODBUS_MESSAGE_IAP_CHECK:
    return _on_receive_IAP_check_message(packet);
  }
  return PROTOCOL_NOT_SUPPORT;
}

WINAPI_EXPORT int WINAPI MODBUS_send_read_coils_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_coils_response_message_pt message)
{
  packet->length = MODBUS_read_coils_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_READ_COILS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_read_discrete_inputs_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_discrete_inputs_response_message_pt message)
{
  packet->length = MODBUS_read_discrete_inputs_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_READ_DISCRETE_INPUTS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_read_holding_registers_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_holding_registers_response_message_pt message)
{
  packet->length = MODBUS_read_holding_registers_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_READ_HOLDING_REGISTERS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_read_input_registers_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_input_registers_response_message_pt message)
{
  packet->length = MODBUS_read_input_registers_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_READ_INPUT_REGISTERS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_write_single_coil_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_single_coil_message_pt message)
{
  packet->length = MODBUS_write_single_coil_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_WRITE_SINGLE_COIL;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_write_single_register_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_single_register_message_pt message)
{
  packet->length = MODBUS_write_single_register_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_WRITE_SINGLE_REGISTER;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_read_exception_status_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_exception_status_response_message_pt message)
{
  packet->length = MODBUS_read_exception_status_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_READ_EXCEPTION_STATUS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_diagnostics_message(modbus_port_pt port, modbus_packet_pt packet, modbus_diagnostics_message_pt message)
{
  packet->length = MODBUS_diagnostics_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_DIAGNOSTICS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_get_common_event_counter_message(modbus_port_pt port, modbus_packet_pt packet, modbus_get_common_event_counter_response_message_pt message)
{
  packet->length = MODBUS_get_common_event_counter_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_GET_COMMON_EVENT_COUNTER;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_get_common_event_log_message(modbus_port_pt port, modbus_packet_pt packet, modbus_get_common_event_log_response_message_pt message)
{
  packet->length = MODBUS_get_common_event_log_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_GET_COMMON_EVENT_LOG;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_write_multiple_coils_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_multiple_coils_response_message_pt message)
{
  packet->length = MODBUS_write_multiple_coils_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_WRITE_MULTIPLE_COILS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_write_multiple_registers_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_multiple_registers_response_message_pt message)
{
  packet->length = MODBUS_write_multiple_registers_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_report_server_id_message(modbus_port_pt port, modbus_packet_pt packet, modbus_report_server_id_response_message_pt message)
{
  packet->length = MODBUS_report_server_id_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_REPORT_SERVER_ID;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_IAP_message(modbus_port_pt port, modbus_packet_pt packet, modbus_IAP_response_message_pt message)
{
  packet->length = MODBUS_IAP_response_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_IAP;
  return MODBUS_PACKET_write(port, packet);
}

WINAPI_EXPORT int WINAPI MODBUS_send_IAP_check_message(modbus_port_pt port, modbus_packet_pt packet, modbus_IAP_check_message_pt message)
{
  packet->length = MODBUS_IAP_check_message_serialize(packet->buffer, message);
  packet->parsed = message;
  packet->code = MODBUS_MESSAGE_IAP_CHECK;
  return MODBUS_PACKET_write(port, packet);
}

extern int BSP_MODBUS_send_error_response(modbus_packet_pt packet, uint8_t status);

#define MODBUS_MESSAGES_PROCESS(_id, oper_name, oper_name_upcase, _name)            \
  __weak int MODBUS##oper_name##_##_name##_message_process(modbus_packet_pt packet) \
  {                                                                                 \
    BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_FUNCTION);                \
    return PROTOCOL_OK;                                                             \
  }

MODBUS_MESSAGES_MAP(MODBUS_MESSAGES_PROCESS)

__weak int MODBUS_error_message_process(modbus_packet_pt packet)
{
  return PROTOCOL_OK;
}
