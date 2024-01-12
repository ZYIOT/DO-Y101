#include "bsp_dac.h"
#include "math.h"

#define BSP_DAC_SIN_WAVE_POINTS 1024
//#define BSP_DAC_SIN_WAVE_POINTS 934

static uint16_t sin_wave_points[BSP_DAC_SIN_WAVE_POINTS];
int BSP_DAC_init(void)
{
    double rad = PI / 180.0;
    double delat = 360.0 / BSP_DAC_SIN_WAVE_POINTS;
    double rate = 3300 / 4096.0;

    for (int i = 0; i < BSP_DAC_SIN_WAVE_POINTS; i++) 
    {
        sin_wave_points[i] = (sin(rad * i * delat) * 800 + 900) / rate; //100R      
        // sin_wave_points[i] = (sin(rad * i * delat) * 200 + 250) / rate; // 20R
    }

    HAL_TIM_Base_Start(&htim6);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    return APP_OK;
}

int BSP_DAC_start(void)
{
    HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);
    HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)sin_wave_points, BSP_DAC_SIN_WAVE_POINTS, DAC_ALIGN_12B_R);
    return APP_OK;
}

int BSP_DAC_stop(void)
{
    HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0);
    HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    return APP_OK;
}
