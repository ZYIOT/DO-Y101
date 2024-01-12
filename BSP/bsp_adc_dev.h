#ifndef __BSP_ADC_DEV_H
#define __BSP_ADC_DEV_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "ADC/ltc2450.h"

#define  TRUE   1
#define  FALSE  0

uint8_t BSP_ADC_WriteByte(uint8_t WriteAddr, uint8_t data);
uint8_t BSP_ADC_ReadByte(uint8_t* data, uint8_t ReadAddr);

void BSP_ADC_DEV_config_mode(void);
float BSP_ADC_DEV_read_resistance(void);

#ifdef __cplusplus
}
#endif

#endif
