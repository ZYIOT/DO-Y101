#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "app.h"
#include "adc.h"
// #define ADC_SAMPLES_POINTS 63 // 当采样周期为7.5时。  一个周期125个点。 有2个通道
// #define ADC_GROUP_SIZE 630    //  采10个周期
#define ADC_SAMPLES_POINTS 89 // 当采样周期为1.5时。  一个周期178个点。 有2个通道
#define ADC_GROUP_SIZE 890    //  采10个周期
#define ADC_CHANNEL_SIZE 2

    int BSP_ADC_init(void);
    int BSP_ADC_get_DO_signals(uint16_t values[ADC_GROUP_SIZE * ADC_CHANNEL_SIZE]);
#ifdef USE_PT1000_ADC
    int BSP_ADC_get_pt1000(uint32_t *value);
#endif
    void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);

    // (A image * B real - A real * B image)/(A real * B real + A image * B image)

#ifdef __cplusplus
}
#endif

#endif
