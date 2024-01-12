#ifndef __APP_MODBUS_H
#define __APP_MODBUS_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "app.h"

#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_TEMPERATURE_OFFSET                          0
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_SATURATION_OFFSET                           8
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_0_PHASE_OFFSET                    4
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_0_TEMPERATURE_OFFSET              6
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_VALUE_OFFSET                                2
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_100_PHASE_OFFSET                  10
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_100_TEMPERATURE_OFFSET            12
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED7_OFFSET                            14
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED8_OFFSET                            16
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_ADDRESS_OFFSET                              18
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_SALINITY_OFFSET                             20
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_CALIBRATE_0_TEMPERATURE_COEFFICIENT_OFFSET  22
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_STERN_VOLMER_COEFFICIENT_OFFSET             24
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_ATMOSPHERE_OFFSET                           26
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_PHASE_OFFSET                                28
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED15_OFFSET                           30
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_TEMPERATURE_COMPENSATION_OFFSET             32
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_VERSION_OFFSET                              34
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_PN_OFFSET                                   40
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_SN_OFFSET                                   48
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_PHASE_DELAY_BASE_OFFSET                     56
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_PHASE_DELAY_COMPENSATION_OFFSET             58
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_RESERVED30_OFFSET                           60
#define APP_MODBUS_EEPROM_HOLDING_REGISTERS_KSV_TEMPERATURE_COEFFICIENT_OFFSET          62

    typedef struct
    {
        uint16_t temperature;                         /* �¶� *10 [0-51��] */
        uint16_t saturation;                          /* ���Ͷ� *10 */
        uint16_t calibrate_0_phase;                   /* ����У׼��λ�� *10000 [0-20000] */
        uint16_t calibrate_0_temperature;             /* ����У׼�¶� *10 [0-51��] */
        uint16_t value;                               /* �ܽ���(mg/L) *100 [0-21] */
        uint16_t calibrate_100_phase;                 /* ������У׼��λ�� *10000 [0-20000] */
        uint16_t calibrate_100_temperature;           /* ������У׼�¶� *10 [0-51��] */
        uint16_t reserved7;                           /* �������� */
        uint16_t reserved8;                           /* Ԥ�� */
        uint16_t address;                             /* RS485��ַ ��Χ0~127 */
        uint16_t salinity;                            /* �ζ�ֵ *10 [0-400] */
        uint16_t calibrate_0_temperature_coefficient; /* ������λ�¶�ϵ�� *10000 [0-1000],Ĭ��:41 */
        uint16_t stern_volmer_coefficient;            /* Stern-Volmerϵ�� */
        uint16_t atmosphere;                          /* ����ѹ�� */
        uint16_t phase;                               /* ��λ�� *10000 [0-20000] */
        uint16_t reserved15;                          /* Ԥ�� */
        uint16_t temperature_compensation;             /* [-32768-32767], 0.0001 */
        uint8_t version[6];                           /* Ӳ����汾 */
        uint8_t PN[8];                                /* �������ͺ� */
        uint8_t SN[8];                                /* SN������ ID ���� ���� */
        uint16_t phase_delay_base;                    /* [0-5000], 0.0001, Ĭ��: 1400 */
        int16_t phase_delay_compensation;             /* [-100-100] */
        uint16_t reserved30;                          /* Ԥ�� */
        uint16_t ksv_temperature_coefficient;         /* Ksv���¶�ϵ�� [0-1000] */
    } app_modbus_holding_registers_t, *app_modbus_holding_registers_pt;

    extern app_modbus_holding_registers_t holding_registers;
    int APP_MODBUS_init(void);
    void APP_MODBUS_task_run(void *argument);
#ifdef __cplusplus
}
#endif
#endif
