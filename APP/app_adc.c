#include <math.h>
#include "kalman_filter.h"
#include "mean_filter.h"
#include "app_adc.h"
#include "bsp_dac.h"
#include "bsp_log.h"
#include "bsp_led.h"
#include "bsp_wdg.h"
#ifndef GPIO_IIC_LIB
#include "bsp_ltc2450.h"
#else
#include "bsp_adc_dev.h"
#endif
#include "app_modbus.h"
#include "stm32_dsp.h"
#include "app_do.h"

#define ADC_MEAN_BUFFER_SIZE 20
#define ADC_PRE_MEAN_BUFFER_SIZE 20
#define PHASE_KALMAN_Q 0.01
#define PHASE_KALMAN_R 1.5

static kalman_filter_t phase_kalman_filter;
static mean_filter_t phase_mean_filter;
static double phase_mean_filter_buffer[ADC_MEAN_BUFFER_SIZE] = {0};
static mean_filter_t phase_pre_mean_filter;
static double phase_pre_mean_filter_buffer[ADC_PRE_MEAN_BUFFER_SIZE] = {0};

static mean_filter_t red_diff_mean_filter;
static double red_diff_mean_filter_buffer[ADC_MEAN_BUFFER_SIZE] = {0};
static mean_filter_t blue_diff_mean_filter;
static double blue_diff_mean_filter_buffer[ADC_MEAN_BUFFER_SIZE] = {0};

static uint16_t adc_buffer[ADC_GROUP_SIZE * ADC_CHANNEL_SIZE] = {0};
static uint16_t adc_values_red[ADC_GROUP_SIZE] = {0};
static uint16_t adc_values_red_ref[ADC_GROUP_SIZE] = {0};
static uint16_t adc_values_blue[ADC_GROUP_SIZE] = {0};
static uint16_t adc_values_blue_ref[ADC_GROUP_SIZE] = {0};

static uint32_t lBufInArray[ADC_GROUP_SIZE]={0};
static uint32_t lBufOutArray[ADC_GROUP_SIZE/2]={0};
static uint32_t lBufMagArray[ADC_GROUP_SIZE/2]={0};

uint16_t MODBUS_DATA_FRAME[32] = {0};

uint16_t flag = 0;
uint16_t data = 0;


static void showDebugAD(uint16_t *adc_values)
{
    for (int i = 0; i < ADC_GROUP_SIZE; i++)
    {
        BSP_LOG_debug("%u ", adc_values[i]);
    }
    BSP_LOG_debug(" \r\n");
}

static float sin_cache[ADC_SAMPLES_POINTS] = {0};
static float cos_cache[ADC_SAMPLES_POINTS] = {0};
typedef struct
{
    double real;
    double imag;
} adc_complex_t;

static int inner_product_63dot(uint16_t *buf, size_t len, adc_complex_t *value)
{
    double imag = 0;
    double real = 0;
    int idx = 0;
    for (size_t i = 0; i < len; i++)
    {
        idx = i % ADC_SAMPLES_POINTS;
        imag += (sin_cache[idx] * buf[i] * 2 / len);
        real += (cos_cache[idx] * buf[i] * 2 / len);
    }
    value->real = sqrt(real * real + imag * imag);
    value->imag = atan2(imag, real);
    return APP_OK;
}

static int inner_product_fftdot(uint16_t *buf, uint16_t len, adc_complex_t *value)
{
    double imag = 0;
    double real = 0;
    // int idx = 0;
    int16_t X1, Y1;
    // uint16_t i;
    uint32_t maxtemp;
    uint16_t pos;
    for (uint16_t i = 0; i < len; i++)
    {
		lBufInArray[i] = buf[i] << 16;
    }
    cr4_fft_1024_stm32(lBufOutArray, lBufInArray, len); 
    for (uint16_t i = 0; i < len / 2; i++)
    {
        X1 = (lBufOutArray[i] << 16) >> 16;
        Y1 = (lBufOutArray[i] >> 16);
        {
            float X = len * ((float)X1) / 32768;
            float Y = len * ((float)Y1) / 32768;
            float Mag = sqrt(X * X + Y * Y) / len;
            lBufMagArray[i] = (uint32_t)(Mag * 65536);
        }
    }
    maxtemp = lBufMagArray[1];
    pos = 1;
    for (uint16_t i = 1;i < len / 2; i++)
    {
        if(maxtemp<lBufMagArray[i])
        {
            maxtemp = lBufMagArray[i];
            pos = i;	
        }
    }
    X1  = (lBufOutArray[pos] << 16) >> 16;//?? 
	Y1  = (lBufOutArray[pos] >> 16);//?? 
	
	float c = len * ((float)X1) / 32768;//??
	float d = len * ((float)Y1) / 32768;//??

    imag= (double) atan2(d, c);		
    real= (double) sqrt(c * c + d * d)*65536/len;
    value->real = real;
    value->imag = imag;
		
    return APP_OK;
}



static void base_data_init()
{
    mean_filter_init(&phase_pre_mean_filter, phase_pre_mean_filter_buffer, ADC_PRE_MEAN_BUFFER_SIZE);
    mean_filter_reset(&phase_pre_mean_filter);
    mean_filter_init(&phase_mean_filter, phase_mean_filter_buffer, ADC_MEAN_BUFFER_SIZE);
    mean_filter_reset(&phase_mean_filter);
    kalman_init(&phase_kalman_filter, 0.36, PHASE_KALMAN_Q, PHASE_KALMAN_R); // 0.6 是空气饱和氧的相位差(tan)

	  mean_filter_init(&red_diff_mean_filter, red_diff_mean_filter_buffer, ADC_MEAN_BUFFER_SIZE);
    mean_filter_reset(&red_diff_mean_filter);
	  mean_filter_init(&blue_diff_mean_filter, blue_diff_mean_filter_buffer, ADC_MEAN_BUFFER_SIZE);
    mean_filter_reset(&blue_diff_mean_filter);
	
    float delta = PI * 2 / ADC_SAMPLES_POINTS;
    for (int i = 0; i < ADC_SAMPLES_POINTS; i++)
    {
        sin_cache[i] = sin(i * delta);
        cos_cache[i] = cos(i * delta);
    }
}

static int get_signals_by_channel(uint16_t adc_buffer[ADC_GROUP_SIZE * ADC_CHANNEL_SIZE], uint8_t ch, uint16_t values[ADC_GROUP_SIZE])
{
    if (ch == 0 || ch > ADC_CHANNEL_SIZE)
    {
        return APP_ERROR;
    }
    for (int i = 0; i < ADC_GROUP_SIZE; i++)
    {
        values[i] = adc_buffer[ch - 1 + i * ADC_CHANNEL_SIZE];
    }
    return APP_OK;
}

static int calculate_do_phase(void)
{
    adc_complex_t red;
    adc_complex_t red_ref;
    adc_complex_t blue;
    adc_complex_t blue_ref;
    double rad = 180 / PI;
    int rc = APP_OK;

    BSP_LED_RED_ON();
    HARDWARE_OS_DELAY_MS(30);
    BSP_DAC_start();
    HARDWARE_OS_DELAY_MS(50);
    rc = BSP_ADC_get_DO_signals(adc_buffer);
    BSP_DAC_stop();
    BSP_LED_RED_OFF();
    APP_CHECK_RC
    get_signals_by_channel(adc_buffer, 1, adc_values_red);
    get_signals_by_channel(adc_buffer, 2, adc_values_red_ref);

    HARDWARE_OS_DELAY_MS(30);
    BSP_LED_BLUE_ON();
    BSP_DAC_start();
    HARDWARE_OS_DELAY_MS(50);
    rc = BSP_ADC_get_DO_signals(adc_buffer);
    BSP_DAC_stop();
    BSP_LED_BLUE_OFF();
    APP_CHECK_RC

    get_signals_by_channel(adc_buffer, 1, adc_values_blue);
    get_signals_by_channel(adc_buffer, 2, adc_values_blue_ref);
		//inner_product_fftdot(adc_values_red, ADC_GROUP_SIZE, &red);
		//inner_product_fftdot(adc_values_red_ref, ADC_GROUP_SIZE, &red_ref);
    inner_product_63dot(adc_values_red, ADC_GROUP_SIZE, &red);
    inner_product_63dot(adc_values_red_ref, ADC_GROUP_SIZE, &red_ref);

		//inner_product_fftdot(adc_values_blue, ADC_GROUP_SIZE, &blue);
		//inner_product_fftdot(adc_values_blue_ref, ADC_GROUP_SIZE, &blue_ref);
    inner_product_63dot(adc_values_blue, ADC_GROUP_SIZE, &blue);
    inner_product_63dot(adc_values_blue_ref, ADC_GROUP_SIZE, &blue_ref);
		
    float red_diff = red_ref.imag - red.imag;
    if (red_diff < 0)
       red_diff *= -1;
		
    float blue_diff = blue_ref.imag - blue.imag;
    if (blue_diff < 0)
        blue_diff *= -1;

		double red_buf = red_diff;
		double blue_buf = blue_diff;
		 mean_filter_add(&red_diff_mean_filter, red_buf);
     mean_filter_get(&red_diff_mean_filter, &red_buf);
		 mean_filter_add(&blue_diff_mean_filter, blue_buf);
    mean_filter_get(&blue_diff_mean_filter, &blue_buf);
   // float diff = red_diff - blue_diff;
		double diff = red_buf - blue_buf;
    double diff_tan = tan(diff);
	  // holding_registers.phase_delay_compensation=  (uint16_t)(10000*diff_tan);
  //  if (diff_tan < 0)
  //      diff_tan *= -1;
    if (diff_tan > 0 && blue.real > 20)
    {
        mean_filter_add(&phase_pre_mean_filter, diff_tan);
			// if(!flag)
		//		{
		//	  holding_registers.phase_delay_compensation=  (uint16_t)(10000*diff_tan);
				holding_registers.reserved8=(uint16_t)(red_diff * rad*100);
			  holding_registers.calibrate_0_temperature_coefficient=(uint16_t)(blue_diff * rad*100);
			//	}
        mean_filter_get(&phase_pre_mean_filter, &diff_tan);
        double kalman_diff_tan = kalman_filter(&phase_kalman_filter, diff_tan);
        double mean_diff_tan = 0;
        mean_filter_add(&phase_mean_filter, kalman_diff_tan);
        mean_filter_get(&phase_mean_filter, &mean_diff_tan);

        BSP_WDG_feed();
			//if(!mean_diff_tan){return APP_ERROR;}
         Do_Value_Update(diff_tan);
			//	data=(uint16_t)(10000*diff_tan);
			//	if(!flag)
			//	{
			//  holding_registers.reserved30=  (uint16_t)(10000*diff_tan);
			//	}
			 // if(((holding_registers.phase_delay_compensation-data)>500)||((data-holding_registers.phase_delay_compensation)>500)){flag=1;}
			//  holding_registers.reserved8=(uint16_t)(red_diff * rad*100);
			//  holding_registers.calibrate_0_temperature_coefficient=(uint16_t)(blue_diff * rad*100);
			  
        BSP_WDG_feed();
        BSP_LOG_debug("\r\n:DO : %d\r\n", MODBUS_DATA_FRAME[ADDR_DO]);
			
        BSP_LOG_debug("0,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,0,%.4f,%.4f,%.4f,%.4f,0,%.4f,%.4f,%.4f,%.4f\r\n",
                       blue.real, diff, diff * rad, diff_tan, kalman_diff_tan, mean_diff_tan,
                       red_ref.real, red.real, red_diff, red_diff * rad,
                       blue_ref.real, blue.real, blue_diff, blue_diff * rad);
        BSP_WDG_feed();
			  flag=0;	
    }
    else
    {
        BSP_LOG_debug("\r\nRED Round  %.4f %.4f : %.4f %.4f\r\n", red_ref.real, red.real, red_diff, red_diff * rad);
        BSP_WDG_feed();
        BSP_LOG_debug("BLUE Round  %.4f %.4f :  %.4f %.4f\r\n", blue_ref.real, blue.real, blue_diff, blue_diff * rad);
        BSP_WDG_feed();
			  flag++;
			if(flag>3){
				flag=10;
				return APP_ERROR;
			          }
    }
    return APP_OK;
}

volatile uint8_t convert_result = APP_ERROR;
void APP_ADC_task_run(void *argument)
{
    BSP_WDG_feed();
    base_data_init();
    int i = 0;
    // for (i = 0; i < 5; i++)
    // {
    //     BSP_WDG_feed();
    //     HARDWARE_OS_DELAY_MS(500);
    // }
    for (;;)
    {
        BSP_WDG_feed();
        HARDWARE_OS_DELAY_MS(500);
        convert_result = calculate_do_phase();
        BSP_WDG_feed();
        HARDWARE_OS_DELAY_MS(500);
    }
}
