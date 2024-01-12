

#ifndef __LTC2450_H__
#define __LTC2450_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "../IIC/gpio_iic.h"

#define LTC2450_BASE_ADDRESS    (0x9 << 3)
#define LTC2450_A0_B0_C0        (0)
#define LTC2450_A1_B0_C0        (1)
#define LTC2450_A0_B1_C0        (2)
#define LTC2450_A1_B1_C0        (3)
#define LTC2450_A0_B0_C1        (4)
#define LTC2450_A1_B0_C1        (5)
#define LTC2450_A0_B1_C1        (6)
#define LTC2450_A1_B1_C1        (7)
#define ADC_DEVICE_ADDRESS      (LTC2450_BASE_ADDRESS) 

#define ADDRESS_TYPE_8_BIT 1
#define ADDRESS_TYPE_16_BIT 2

typedef int (*IIC_LTC2450_read_fun)(void *handle, uint8_t device_id, uint8_t *data, size_t timeout);
typedef int (*IIC_LTC2450_write_fun)(void *handle, uint8_t device_id, uint8_t *data, size_t timeout);
typedef int (*IIC_LTC2450_read_bytes_fun)(void *handle, uint8_t device_id, uint8_t *data, size_t len, size_t timeout);
typedef int (*IIC_LTC2450_write_bytes_fun)(void *handle, uint8_t device_id, uint8_t *data, size_t len, size_t timeout);


typedef struct 
{
    IIC_LTC2450_read_fun read;
    IIC_LTC2450_write_fun write;
    IIC_LTC2450_read_bytes_fun reads;
    IIC_LTC2450_write_bytes_fun writes;
}LTC2450_FUN_TypeDef;

typedef struct
{
    GPIO_IIC_TypeDef *handle;
    LTC2450_FUN_TypeDef *fun;
    uint8_t address_type;
} GPIO_LTC2450_TypeDef;

extern LTC2450_FUN_TypeDef LTC2450_Fun;

#ifdef __cplusplus
}
#endif


#endif

