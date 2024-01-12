#include <math.h>
#include "kalman_filter.h"
#include "mean_filter.h"
#include "app_temperature.h"
#ifndef GPIO_IIC_LIB
#include "bsp_ltc2450.h"
#else
#include "bsp_adc_dev.h"
#endif
#include "bsp_log.h"

#include "stm32_dsp.h"
#include "app_do.h"

#define PT1000_MEAN_BUFFER_SIZE 10
#define PT1000_KALMAN_Q 0.001
#define PT1000_KALMAN_R 1.5


static kalman_filter_t pt1000_kalman_filter;
static mean_filter_t pt1000_mean_filter;
static double pt1000_mean_filter_buffer[PT1000_MEAN_BUFFER_SIZE] = {0};

extern uint16_t MODBUS_DATA_FRAME[32];

static void base_temperature_data_init(void)
{
    uint32_t pt1000 = 0;
	BSP_ADC_DEV_config_mode();
    mean_filter_init(&pt1000_mean_filter, pt1000_mean_filter_buffer, PT1000_MEAN_BUFFER_SIZE);
    mean_filter_reset(&pt1000_mean_filter);
    kalman_init(&pt1000_kalman_filter, (double)pt1000, PT1000_KALMAN_Q, PT1000_KALMAN_R); //

}


static double pt1000_adc_to_temp(double adc)
{
    return ((adc * 20000) / (65535 - adc) - 1000) / 3.851;
}

void APP_TEMPERATURE_task_run(void *argument)
{
    size_t tick = HARDWARE_GET_TICK();
    BSP_WDG_feed();
    base_temperature_data_init();
    for(;;)
    {
        float pt1000 = 0;
        pt1000 = BSP_ADC_DEV_read_resistance();
        double kalman_pt1000 = kalman_filter(&pt1000_kalman_filter, pt1000);
        double mean_pt1000 = 0;
        mean_filter_add(&pt1000_mean_filter, pt1000);
        mean_filter_get(&pt1000_mean_filter, &mean_pt1000);
        // double temperature = pt1000_adc_to_temp(mean_pt1000);
        Temp_Value_Update((float) mean_pt1000);
        if(HARDWARE_GET_TICK() - tick > 1000)
        {
            tick = HARDWARE_GET_TICK();
#ifdef SHOW_ADC_TEMPERATURE
            BSP_LOG_trace("pt1000=%.3f\r\n", mean_pt1000);
#endif        
            BSP_LOG_debug("\r\n temprature : %d\r\n", MODBUS_DATA_FRAME[ADDR_TEMP]);
        }
        HARDWARE_OS_DELAY_MS(50);
				BSP_WDG_feed();
    }
}

