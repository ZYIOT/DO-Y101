#include "bsp_adc.h"

static uint16_t _adc1_buffer[ADC_GROUP_SIZE * ADC_CHANNEL_SIZE];
static uint8_t adc1_has_data = 0;
int BSP_ADC_init(void)
{
	if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
#ifdef USE_PT1000_ADC
	if (HAL_ADCEx_Calibration_Start(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_ADC_Start(&hadc2);
#endif
	// HAL_ADC_Start_DMA(&hadc1, (uint32_t *)_adc1_buffer, ADC_GROUP_SIZE * ADC_CHANNEL_SIZE) == HAL_OK ? APP_OK : APP_ERROR;

	return APP_OK;
}

int BSP_ADC_get_DO_signals(uint16_t values[ADC_GROUP_SIZE * ADC_CHANNEL_SIZE])
{
	int rc = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)_adc1_buffer, ADC_GROUP_SIZE * ADC_CHANNEL_SIZE) == HAL_OK ? APP_OK : APP_ERROR;
	APP_CHECK_RC
	uint32_t tickstart = HARDWARE_GET_TICK();
	while (adc1_has_data == 0)
	{
		if ((HARDWARE_GET_TICK() - tickstart) > 100)
		{
			HAL_ADC_Stop_DMA(&hadc1);
			return APP_TIMEOUT;
		}
		HARDWARE_OS_DELAY_MS(5);
	}
	HAL_ADC_Stop_DMA(&hadc1);
	memcpy(values, _adc1_buffer, ADC_GROUP_SIZE * ADC_CHANNEL_SIZE * 2);
	adc1_has_data = 0;
	return APP_OK;
}

#ifdef USE_PT1000_ADC
int BSP_ADC_get_pt1000(uint32_t *value)
{
	HAL_ADC_Start(&hadc2);

	if (HAL_ADC_PollForConversion(&hadc2, 100) != HAL_OK)
	{
		return APP_ERROR;
	}
	if (HAL_ADC_GetState(&hadc2) == HAL_ADC_STATE_REG_EOC)
	{
		return APP_ERROR;
	}
	*value = HAL_ADC_GetValue(&hadc2);
	return APP_OK;
}
#endif

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1)
	{
		adc1_has_data = 1;
	}
}
