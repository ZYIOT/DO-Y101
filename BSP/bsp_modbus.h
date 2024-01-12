#ifndef __BSP_MODBUS_H
#define __BSP_MODBUS_H
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdint.h>
#include "bsp_usart.h"
#include "modbus_slave.h"

    extern modbus_port_t modbus_port;
    int BSP_MODBUS_init(void);
    int BSP_MODBUS_send_error_response(modbus_packet_pt packet, uint8_t status);

#define BSP_MODBUS_PACKET_read(packet) MODBUS_PACKET_read(&modbus_port, packet)
#define BSP_MODBUS_PACKET_write(packet) MODBUS_PACKET_write(&modbus_port, packet)
#define BSP_MODBUS_receive_packet(packet) MODBUS_receive_packet(packet)

#define BSP_MODBUS_send_read_coils_message(packet, message) MODBUS_send_read_coils_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_read_discrete_inputs_message(packet, message) MODBUS_send_read_discrete_inputs_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_read_holding_registers_message(packet, message) MODBUS_send_read_holding_registers_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_read_input_registers_message(packet, message) MODBUS_send_read_input_registers_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_write_single_coil_message(packet, message) MODBUS_send_write_single_coil_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_write_single_register_message(packet, message) MODBUS_send_write_single_register_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_read_exception_status_message(packet, message) MODBUS_send_read_exception_status_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_diagnostics_message(packet, message) MODBUS_send_diagnostics_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_get_common_event_counter_message(packet, message) MODBUS_send_get_common_event_counter_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_get_common_event_log_message(packet, message) MODBUS_send_get_common_event_log_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_write_multiple_coils_message(packet, message) MODBUS_send_write_multiple_coils_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_write_multiple_registers_message(packet, message) MODBUS_send_write_multiple_registers_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_report_server_id_message(packet, message) MODBUS_send_report_server_id_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_IAP_message(packet, message) MODBUS_send_IAP_message(&modbus_port, packet, message)
#define BSP_MODBUS_send_IAP_check_message(packet, message) MODBUS_send_IAP_check_message(&modbus_port, packet, message)

#ifdef __cplusplus
}
#endif
#endif
