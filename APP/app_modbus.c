#include "hardware_wrapper.h"
#include "bsp_modbus.h"
#include "bsp_log.h"
#include "bsp_mcu.h"
#include "app_modbus.h"
#include "app_config.h"
#include "app_adc.h"
#include "app_do.h"

app_modbus_holding_registers_t holding_registers = {0};

#define APP_MODBUS_HOLDING_REGISTERS_SIZE 56
uint8_t holding_registers_buffer[APP_MODBUS_HOLDING_REGISTERS_SIZE] = {0};

extern volatile uint8_t convert_result;

static int _write_IAP(uint16_t value)
{
    switch (value)
{
    case 1:
    case 2:
    case 3:
    case 11:
        BSP_LOG_debug("... ... reboot\r\n");
        HARDWARE_OS_DELAY_MS(100);
        return APP_OK; //send ok first and reboot mcu
    default:
        return APP_ERROR;
    }
}



static int _write_calibrate(uint16_t value)
{
    switch (value)
    {
        case 1:
            return DO_Calibration(value);
        case 2:
            return DO_Calibration(value);
        case 3:
        case 10:
            return DO_Calibration(value);
        case 11:
            return APP_OK; //send ok first and reboot mcu
        case 0x80:
            HARDWARE_OS_DELAY_MS(10);
            BSP_MCU_reset();
            return APP_OK;
        case 0xa5:
            return BOOTLOADER_erase_app2();
        default:
            return APP_ERROR;
    }
}

static int _write_device_address(uint16_t value)
{
    if (value >= 1 && value < 128)
    {
        holding_registers.address = value;
        return APP_CONFIG_write_device_info();
    }
    return APP_ERROR;
}

static void frozen_pn_and_fireware(app_modbus_holding_registers_pt message)
{
    message->version[3] = APP_FIRMWARE_MAJOR;
    message->version[4] = APP_FIRMWARE_MINOR;
    message->version[5] = APP_FIRMWARE_REVISION;
    memset(message->PN, 0, 8);
    memcpy(message->PN, PN_CODE, strlen(PN_CODE));
    write_uint16_t_BE(SN_CODE, &message->SN[0]);
    message->SN[2] = 1;
}

int APP_MODBUS_holding_registers_init(app_modbus_holding_registers_pt message)
{
    message->temperature = 0;
    message->saturation = 0;
    message->calibrate_0_phase = 0;
    message->calibrate_0_temperature = 0;
    message->value = 0;
    message->calibrate_100_phase = 0;
    message->calibrate_100_temperature = 0;
    message->reserved7 = 0;
    message->reserved8 = 32;
    message->address = APP_CONFIG_device_id();
    message->salinity = 0;
    message->calibrate_0_temperature_coefficient = 44;
    message->stern_volmer_coefficient = 0;
    message->atmosphere = 0;
    message->phase = 0;
    message->reserved15 = 0;
    message->temperature_compensation = 0;
    memset(message->version, 0, 6);
    memset(message->PN, 0, 8);
    memset(message->SN, 0, 8);
    frozen_pn_and_fireware(message);
    message->phase_delay_base = 0;
    message->phase_delay_compensation = 0;
    message->reserved30 = 0;
    message->ksv_temperature_coefficient = 0;
    return APP_OK;
}

static void APP_MODBUS_holding_registers_serialize(uint8_t bytes[APP_MODBUS_HOLDING_REGISTERS_SIZE], app_modbus_holding_registers_pt message)
{
    write_uint16_t_BE(message->temperature, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_TEMPERATURE_OFFSET]);
    write_uint16_t_BE(message->saturation, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_SATURATION_OFFSET]);
    write_uint16_t_BE(message->calibrate_0_phase, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_0_PHASE_OFFSET]);
    write_uint16_t_BE(message->calibrate_0_temperature, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_0_TEMPERATURE_OFFSET]);   
	  write_uint16_t_BE(message->value, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_VALUE_OFFSET]);  
	  write_uint16_t_BE(message->calibrate_100_phase, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_100_PHASE_OFFSET]);
    write_uint16_t_BE(message->calibrate_100_temperature, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_100_TEMPERATURE_OFFSET]);
    write_uint16_t_BE(message->reserved7, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED7_OFFSET]);
    write_uint16_t_BE(message->reserved8, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED8_OFFSET]);
    write_uint16_t_BE(message->address, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_ADDRESS_OFFSET]);
    write_uint16_t_BE(message->salinity, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_SALINITY_OFFSET]);
    write_uint16_t_BE(message->calibrate_0_temperature_coefficient, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_0_TEMPERATURE_COEFFICIENT_OFFSET]);
    write_uint16_t_BE(message->stern_volmer_coefficient, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_STERN_VOLMER_COEFFICIENT_OFFSET]);
    write_uint16_t_BE(message->atmosphere, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_ATMOSPHERE_OFFSET]);
    write_uint16_t_BE(message->phase, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_PHASE_OFFSET]);
    write_uint16_t_BE(message->reserved15, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED15_OFFSET]);
    write_int16_t_BE(message->temperature_compensation, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_TEMPERATURE_COMPENSATION_OFFSET]);
    memcpy(&bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_VERSION_OFFSET], message->version, 6);
    memcpy(&bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_PN_OFFSET], message->PN, 8);
    memcpy(&bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_SN_OFFSET], message->SN, 8);
    write_uint16_t_BE(message->phase_delay_base, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_PHASE_DELAY_BASE_OFFSET]);
    write_int16_t_BE(message->phase_delay_compensation, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_PHASE_DELAY_COMPENSATION_OFFSET]);
    write_uint16_t_BE(message->reserved30, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED30_OFFSET]);
    write_uint16_t_BE(message->ksv_temperature_coefficient, &bytes[APP_MODBUS_EEPROM_HOLDING_REGISTERS_KSV_TEMPERATURE_COEFFICIENT_OFFSET]);
}

int MODBUS_read_holding_registers_message_process(modbus_packet_pt packet)
{
    modbus_read_holding_registers_request_message_pt pmsg = (modbus_read_holding_registers_request_message_pt)packet->parsed;
    if (pmsg->quantity == 0 || pmsg->address + pmsg->quantity > 32)
    {
        BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_DATA_ADDRESS);
        return PROTOCOL_ERROR;
    }
    modbus_read_holding_registers_response_message_t message;
    MODBUS_read_holding_registers_response_message_init(&message);
    message.count = pmsg->quantity * 2;
    if((APP_OK != convert_result) && ((pmsg->address <= ADDR_DO) && (pmsg->address + pmsg->quantity >= ADDR_DO)))
    {
        BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_DATA_VALUE);
        return PROTOCOL_ERROR;
    }
    APP_MODBUS_holding_registers_serialize(holding_registers_buffer, &holding_registers);
    memcpy(message.buffer, &holding_registers_buffer[pmsg->address * 2], pmsg->quantity * 2);
    BSP_MODBUS_send_read_holding_registers_message(packet, &message);
    return PROTOCOL_OK;
}

int MODBUS_write_multiple_registers_message_process(modbus_packet_pt packet)
{
    modbus_write_multiple_registers_request_message_pt pmsg = (modbus_write_multiple_registers_request_message_pt)packet->parsed;
    if (pmsg->quantity == 0 || pmsg->quantity * 2 != pmsg->count)
    {
        BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_DATA_ADDRESS);
        return PROTOCOL_ERROR;
    }
    if (pmsg->address < 17 || pmsg->address + pmsg->quantity > 28)
    {
        BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_DATA_ADDRESS);
        return PROTOCOL_ERROR;
    }
    uint8_t bytes[APP_MODBUS_HOLDING_REGISTERS_SIZE];
    APP_MODBUS_holding_registers_serialize(holding_registers_buffer, &holding_registers);
    memcpy(bytes, &holding_registers_buffer[APP_MODBUS_EEPROM_HOLDING_REGISTERS_VERSION_OFFSET], 22);

    memcpy(&bytes[(pmsg->address - 17) * 2], pmsg->value, pmsg->count);
    memcpy(holding_registers.version, &bytes[0], 6);
    memcpy(holding_registers.PN, &bytes[6], 8);
    memcpy(holding_registers.SN, &bytes[14], 8);
    frozen_pn_and_fireware(&holding_registers);
    int rc = APP_CONFIG_write_device_info();
    if (rc != APP_OK)
    {
        BSP_MODBUS_send_error_response(packet, MODBUS_SERVICE_DEVICE_FAILURE);
        return PROTOCOL_ERROR;
    }
    modbus_write_multiple_registers_response_message_t message;
    MODBUS_write_multiple_registers_response_message_init(&message);
    message.quantity = pmsg->quantity;
    message.address = pmsg->address;

    BSP_MODBUS_send_write_multiple_registers_message(packet, &message);
    return APP_OK;
}

int MODBUS_write_single_register_message_process(modbus_packet_pt packet)
{
    modbus_write_single_register_message_pt pmsg = (modbus_write_single_register_message_pt)packet->parsed;
    int rc = 0;
    switch (pmsg->address)
    {
		case 4:
            rc = _write_IAP(pmsg->value);
            break;
        case 7:
            rc = _write_calibrate(pmsg->value);
            break;
				case 8:
            rc = DO_Write_100_temperature_coefficient(pmsg->value);
            break;
        case 9:
            rc = _write_device_address(pmsg->value);
            break;
        case 10:
            rc = DO_Write_Salinity(pmsg->value);
            break;
        case 11:
            rc = DO_Write_0_temperature_coefficient(pmsg->value);
            break;
        case 13:
            rc = DO_Write_atmosphere(pmsg->value);
            break;
        case 15:
            rc = DO_Write_temperature_flag(pmsg->value);
            break;
        case 16:
            rc = DO_Write_temperature_compensation(pmsg->value);
            break;
        case 28:
            rc = DO_Write_phase_delay_base(pmsg->value);
            break;
        case 29:
            rc = DO_Write_phase_delay_compensation(pmsg->value);
            break;
        case 30:
            rc = DO_Write_phase_flag(pmsg->value);
            break;
        case 31:
            rc = DO_Write_ksv_temperature_coefficient(pmsg->value);
            break;
        default:
            BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_DATA_ADDRESS);
            return PROTOCOL_ERROR;
    }
    if (rc != APP_OK)
    {
        BSP_MODBUS_send_error_response(packet, MODBUS_ILLEGAL_DATA_VALUE);
        return PROTOCOL_ERROR;
    }
    BSP_MODBUS_send_write_single_register_message(packet, pmsg);
    if (pmsg->address == 4 && pmsg->value == 11)
    {
        HARDWARE_OS_DELAY_MS(10);
        BSP_MCU_reset();
    }
    return PROTOCOL_OK;
}

int APP_MODBUS_init(void)
{
    APP_MODBUS_holding_registers_init(&holding_registers);
    APP_CONFIG_init();
    frozen_pn_and_fireware(&holding_registers);
    BSP_LOG_debug("Sensor Dev_ID = %d\r\n", holding_registers.address);
    return APP_OK;
}

void APP_MODBUS_task_run(void *argument)
{
    modbus_packet_t packet;
    int rc = 0;
    BSP_MODBUS_init();
    for (;;)
    {
        MODBUS_PACKET_init(&packet);
        rc = BSP_MODBUS_PACKET_read(&packet);
        if (rc == PROTOCOL_OK)
        {
            BSP_MODBUS_receive_packet(&packet);
        }
        else
        {
            HARDWARE_OS_DELAY_MS(10);
        }
    }
}
