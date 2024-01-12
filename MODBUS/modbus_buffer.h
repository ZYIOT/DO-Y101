#ifndef __MODBUS_BUFFER_H
#define __MODBUS_BUFFER_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "modbus_common.h"

    typedef struct
    {
        uint8_t buffer[MODBUS_PROTOCOL_BUFFER_SIZE];
        uint32_t position;  // �ϴ��Ѵ�������λ�õ�ƫ��
        uint32_t read_position; // ���ֶ����ݵ�λ��ƫ��
        uint32_t size;      // ʣ��δ�������ݸ���
        uint32_t read_size; // ���ֽ��������ݸ���
        modbus_port_pt port;
    } modbus_buffer_t, *modbus_buffer_pt;

#define MODBUS_BUFFER_OK 0
#define MODBUS_BUFFER_FULL -1
#define MODBUS_BUFFER_NO_SUFFICIENT_SPACE -2

    int MODBUS_BUFFER_init(modbus_buffer_pt buffer);
    uint32_t MODBUS_BUFFER_count(modbus_buffer_pt buffer);
    uint32_t MODBUS_BUFFER_get_free_space(modbus_buffer_pt buffer);
    int MODBUS_BUFFER_read(modbus_buffer_pt buffer, uint8_t *data, uint32_t len);
    // int MODBUS_BUFFER_write(modbus_buffer_pt buffer, uint8_t *data, uint32_t len);
    int MODBUS_BUFFER_reset(modbus_buffer_pt buffer);
    int MODBUS_BUFFER_shift(modbus_buffer_pt buffer);
#ifdef __cplusplus
}
#endif
#endif
