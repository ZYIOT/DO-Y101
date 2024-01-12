#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "modbus_common.h"
#include "modbus_packet.h"
#include "modbus_message.h"

  WINAPI_EXPORT int WINAPI MODBUS_receive_packet(modbus_packet_pt packet);
  WINAPI_EXPORT int WINAPI MODBUS_send_read_coils_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_coils_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_read_discrete_inputs_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_discrete_inputs_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_read_holding_registers_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_holding_registers_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_read_input_registers_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_input_registers_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_write_single_coil_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_single_coil_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_write_single_register_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_single_register_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_read_exception_status_message(modbus_port_pt port, modbus_packet_pt packet, modbus_read_exception_status_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_diagnostics_message(modbus_port_pt port, modbus_packet_pt packet, modbus_diagnostics_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_get_common_event_counter_message(modbus_port_pt port, modbus_packet_pt packet, modbus_get_common_event_counter_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_get_common_event_log_message(modbus_port_pt port, modbus_packet_pt packet, modbus_get_common_event_log_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_write_multiple_coils_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_multiple_coils_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_write_multiple_registers_message(modbus_port_pt port, modbus_packet_pt packet, modbus_write_multiple_registers_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_report_server_id_message(modbus_port_pt port, modbus_packet_pt packet, modbus_report_server_id_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_IAP_message(modbus_port_pt port, modbus_packet_pt packet, modbus_IAP_response_message_pt message);
  WINAPI_EXPORT int WINAPI MODBUS_send_IAP_check_message(modbus_port_pt port, modbus_packet_pt packet, modbus_IAP_check_message_pt message);

#define MODBUS_MESSAGES_MAP(F)                                             \
  F(MODBUS_MESSAGE_READ_COILS, , , read_coils)                             \
  F(MODBUS_MESSAGE_READ_DISCRETE_INPUTS, , , read_discrete_inputs)         \
  F(MODBUS_MESSAGE_READ_HOLDING_REGISTERS, , , read_holding_registers)     \
  F(MODBUS_MESSAGE_READ_INPUT_REGISTERS, , , read_input_registers)         \
  F(MODBUS_MESSAGE_WRITE_SINGLE_COIL, , , write_single_coil)               \
  F(MODBUS_MESSAGE_WRITE_SINGLE_REGISTER, , , write_single_register)       \
  F(MODBUS_MESSAGE_READ_EXCEPTION_STATUS, , , read_exception_status)       \
  F(MODBUS_MESSAGE_DIAGNOSTICS, , , diagnostics)                           \
  F(MODBUS_MESSAGE_GET_COMMON_EVENT_COUNTER, , , get_common_event_counter) \
  F(MODBUS_MESSAGE_GET_COMMON_EVENT_LOG, , , get_common_event_log)         \
  F(MODBUS_MESSAGE_WRITE_MULTIPLE_COILS, , , write_multiple_coils)         \
  F(MODBUS_MESSAGE_WRITE_MULTIPLE_REGISTERS, , , write_multiple_registers) \
  F(MODBUS_MESSAGE_REPORT_SERVER_ID, , , report_server_id)                 \
  F(MODBUS_MESSAGE_IAP, , , IAP)                                           \
  F(MODBUS_MESSAGE_IAP_CHECK, , , IAP_check)

#define MODBUS_MESSAGES_PROCESS_DECLARE(_id, oper_name, oper_name_upcase, _name) \
  int MODBUS##oper_name##_##_name##_message_process(modbus_packet_pt packet);

  MODBUS_MESSAGES_MAP(MODBUS_MESSAGES_PROCESS_DECLARE)
  int MODBUS_error_message_process(modbus_packet_pt packet);
#ifdef __cplusplus
}
#endif
#endif
