#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "app.h"
#include "adc.h"
// #define ADC_SAMPLES_POINTS 63 // ����������Ϊ7.5ʱ��  һ������125���㡣 ��2��ͨ��
// #define ADC_GROUP_SIZE 630    //  ��10������
#define ADC_SAMPLES_POINTS 89 // ����������Ϊ1.5ʱ��  һ������178���㡣 ��2��ͨ��
#define ADC_GROUP_SIZE 890    //  ��10������
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
