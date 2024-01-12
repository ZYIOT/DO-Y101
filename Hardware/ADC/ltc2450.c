#include "ltc2450.h"
#include "../IIC/gpio_iic.h"
#include "hardware_wrapper.h"


#define I2Cx        (device->handle)
#define addr_type   (device->address_type)
#define WRITE_ADDR(addr)    (addr << 1)
#define READ_ADDR(addr)     (1 + (addr << 1))
#define __CHECK_RC_AND_RETURN \
    if (rc != 0)              \
    {                         \
        GPIO_IIC_stop(hi2c);  \
        return rc;            \
    }


// 使用 GPIO 模拟 IIC 的读写操作函数

static int _check_busy(GPIO_LTC2450_TypeDef *device, uint8_t device_id, size_t timeout)
{
    return 0;
}


static int _GPIO_I2C_Master_Receive(GPIO_IIC_TypeDef *hi2c,  uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    int rc = 0;
    size_t start = HARDWARE_GET_TICK();
    GPIO_IIC_start(hi2c);
    GPIO_IIC_send_byte(hi2c, READ_ADDR(DevAddress));
    rc = GPIO_IIC_wait_ack(hi2c);
    __CHECK_RC_AND_RETURN
    for (size_t i = 0; i < Size; i++)
    {
        pData[i] = GPIO_IIC_read_byte(hi2c, i == (Size - 1) ? GPIO_IIC_NACK : GPIO_IIC_ACK);
        if(HARDWARE_GET_TICK() - start > Timeout)
        {
            rc = -1;
            __CHECK_RC_AND_RETURN
        }
    }
    GPIO_IIC_stop(hi2c);
    return HAL_OK;
}

static int _GPIO_I2C_Master_Transmit(GPIO_IIC_TypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
    int rc = 0;
    size_t start = HARDWARE_GET_TICK();
    GPIO_IIC_start(hi2c);
    GPIO_IIC_send_byte(hi2c, WRITE_ADDR(DevAddress));
    rc = GPIO_IIC_wait_ack(hi2c);
    __CHECK_RC_AND_RETURN
    for (size_t i = 0; i < Size; i++)
    {
        GPIO_IIC_send_byte(hi2c, pData[i]);
        rc = GPIO_IIC_wait_ack(hi2c);
        __CHECK_RC_AND_RETURN
        if(HARDWARE_GET_TICK() - start > Timeout)
        {
            rc = -1;
            __CHECK_RC_AND_RETURN
        }
    }
    GPIO_IIC_stop(hi2c);
    return HAL_OK;
}

static int _GPIO_IIC_LTC2450_read_bytes(GPIO_LTC2450_TypeDef *device, uint8_t device_id, uint8_t *data, size_t len, size_t timeout)
{
    uint8_t device_addr = ADC_DEVICE_ADDRESS | (device_id & 0x7);
    int rc = 0;
    rc = _GPIO_I2C_Master_Receive(I2Cx, device_addr, data, len, timeout);
    return rc;
}

static int _GPIO_IIC_LTC2450_write_bytes(GPIO_LTC2450_TypeDef *device, uint8_t device_id, uint8_t *data, size_t len, size_t timeout)
{
    uint8_t device_addr = ADC_DEVICE_ADDRESS | (device_id & 0x7);
    int rc = 0;
    rc = _GPIO_I2C_Master_Transmit(I2Cx, device_addr, data, len, timeout);

    return rc;
}

static int _GPIO_IIC_LTC2450_read(GPIO_LTC2450_TypeDef *device, uint8_t device_id, uint16_t addr, uint8_t *data, size_t timeout)
{
    return _GPIO_IIC_LTC2450_read_bytes(device, device_id, data, 1, timeout);
}

static int _GPIO_IIC_LTC2450_write(GPIO_LTC2450_TypeDef *device, uint8_t device_id, uint16_t addr, uint8_t *data, size_t timeout)
{
    return _GPIO_IIC_LTC2450_write_bytes(device, device_id, data, 1, timeout);
}


LTC2450_FUN_TypeDef LTC2450_Fun = 
{
    .read = _GPIO_IIC_LTC2450_read,
    .write = _GPIO_IIC_LTC2450_write,
    .reads = _GPIO_IIC_LTC2450_read_bytes,
    .writes = _GPIO_IIC_LTC2450_write_bytes,
};
