#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "app.h"
#include "app_config.h"
#include "bsp_flash.h"
#include "bsp_log.h"
#include "bsp_wdg.h"
#include "bsp_adc.h"
#include "utils.h"
#include "hardware_wrapper.h"

static int APP_CONFIG_read(uint8_t page, uint8_t bytes[APP_CONFIG_SETTINGS_LENGTH])
{
    memcpy(bytes, (uint8_t *)APP_CONFIG_SETTINGS_ADDRESS + (page * APP_CONFIG_SETTINGS_LENGTH), APP_CONFIG_SETTINGS_LENGTH);
    uint32_t crc = crc32(bytes, APP_CONFIG_SETTINGS_LENGTH - 4);
    if (crc != read_uint32_t(&bytes[APP_CONFIG_SETTINGS_LENGTH - 4]))
    {
        return APP_ERROR;
    }
    return APP_OK;
}

static int APP_CONFIG_write(uint8_t page, uint8_t bytes[APP_CONFIG_SETTINGS_LENGTH])
{
    uint32_t crc = crc32(bytes, APP_CONFIG_SETTINGS_LENGTH - 4);
    write_uint32_t(crc, &bytes[APP_CONFIG_SETTINGS_LENGTH - 4]);
    int rc = BSP_FLASH_erase(APP_CONFIG_SETTINGS_ADDRESS + (page * APP_CONFIG_SETTINGS_LENGTH), APP_CONFIG_SETTINGS_LENGTH);
    APP_CHECK_RC
    return BSP_FLASH_flush(0, (uint32_t *)bytes, APP_CONFIG_SETTINGS_LENGTH, APP_CONFIG_SETTINGS_ADDRESS + (page * APP_CONFIG_SETTINGS_LENGTH), APP_CONFIG_SETTINGS_LENGTH);
}

#define SETTINGS_PAGE_OFFSET 0
#define DEVICE_INFO_PAGE_OFFSET 1

int APP_CONFIG_read_settings(void)
{
    uint8_t bytes[APP_CONFIG_SETTINGS_LENGTH] = {0};
    int rc = APP_CONFIG_read(SETTINGS_PAGE_OFFSET, bytes);

    APP_CHECK_RC
    int pos = 0;
    holding_registers.calibrate_0_phase = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.calibrate_0_temperature = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.calibrate_100_phase = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.calibrate_100_temperature = read_uint16_t(&bytes[pos]);
	pos += 2;
    holding_registers.reserved8 = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.salinity = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.calibrate_0_temperature_coefficient = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.stern_volmer_coefficient = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.atmosphere = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.reserved15 = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.temperature_compensation = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.phase_delay_base = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.phase_delay_compensation = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.reserved30 = read_uint16_t(&bytes[pos]);
    pos += 2;
    holding_registers.ksv_temperature_coefficient = read_uint16_t(&bytes[pos]);
    return APP_OK;
}

int APP_CONFIG_write_settings(void)
{
    uint8_t bytes[APP_CONFIG_SETTINGS_LENGTH] = {0};
    int pos = 0;
    write_uint16_t(holding_registers.calibrate_0_phase, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.calibrate_0_temperature, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.calibrate_100_phase, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.calibrate_100_temperature, &bytes[pos]);
	pos += 2;
    write_uint16_t(holding_registers.reserved8, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.salinity, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.calibrate_0_temperature_coefficient, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.stern_volmer_coefficient, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.atmosphere, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.reserved15, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.temperature_compensation, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.phase_delay_base, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.phase_delay_compensation, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.reserved30, &bytes[pos]);
    pos += 2;
    write_uint16_t(holding_registers.ksv_temperature_coefficient, &bytes[pos]);
    return APP_CONFIG_write(SETTINGS_PAGE_OFFSET, bytes);
}

int APP_CONFIG_read_device_info(void)
{
    uint8_t bytes[APP_CONFIG_SETTINGS_LENGTH] = {0};
    int rc = APP_CONFIG_read(DEVICE_INFO_PAGE_OFFSET, bytes);
    APP_CHECK_RC
    holding_registers.address = bytes[0];
    int pos = 1;
    memcpy(holding_registers.version, &bytes[pos], 6);
    pos += 6;
    memcpy(holding_registers.PN, &bytes[pos], 8);
    pos += 8;
    memcpy(holding_registers.SN, &bytes[pos], 8);
    return APP_OK;
}

int APP_CONFIG_write_device_info(void)
{
    uint8_t bytes[APP_CONFIG_SETTINGS_LENGTH] = {0};
    int pos = 1;
    bytes[0] = holding_registers.address;
    memcpy(&bytes[pos], holding_registers.version, 6);
    pos += 6;
    memcpy(&bytes[pos], holding_registers.PN, 8);
    pos += 8;
    memcpy(&bytes[pos], holding_registers.SN, 8);
    return APP_CONFIG_write(DEVICE_INFO_PAGE_OFFSET, bytes);
}

int APP_CONFIG_init(void)
{
    if (APP_CONFIG_read_settings() != APP_OK)
    {
        holding_registers.calibrate_0_phase = 13000;
        holding_registers.calibrate_0_temperature = 250;
        holding_registers.calibrate_100_phase = 3800;
        holding_registers.calibrate_100_temperature = 250;
        
			  holding_registers.reserved8=32;
        holding_registers.salinity = 0;
        holding_registers.calibrate_0_temperature_coefficient = 44;
        holding_registers.stern_volmer_coefficient = 4000;
        holding_registers.atmosphere = 1013;
        holding_registers.reserved15 = 0;
        holding_registers.temperature_compensation = 0;
        holding_registers.phase_delay_base = 1600;
        holding_registers.phase_delay_compensation = 0;
        holding_registers.reserved30 = 0;
        holding_registers.ksv_temperature_coefficient = 280;
    }
    if (APP_CONFIG_read_device_info() != APP_OK)
    {
        holding_registers.address = 1;
        memset(holding_registers.version, 0, 6);
        holding_registers.version[0] = APP_HARDWARE_MAJOR;
        holding_registers.version[1] = APP_HARDWARE_MINOR;
        holding_registers.version[2] = APP_HARDWARE_REVISION;
        memset(holding_registers.PN, 0, 8);
        memset(holding_registers.SN, 0, 8);
    }
    if (holding_registers.address == 0 || holding_registers.address > 127)
    {
        holding_registers.address = 1;
    }
    return APP_OK;
}

uint8_t APP_CONFIG_device_id(void)
{
    return holding_registers.address;
}

