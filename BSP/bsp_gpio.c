#include "bsp_gpio.h"

void BSP_GPIO_set_pin_as_input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t i;
	for (i = 0x00; i < MAX_GPIO_INDEX; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			if(i < 8)
			{
				// GPIOx->CRL = (GPIOx->CRL & ~(BSP_GPIO_MOVE_CRL(BSP_GPIO_ALL_MASK, i))) | BSP_GPIO_MOVE_CRL(BSP_GPIO_INPUT, i);
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRL, BSP_GPIO_MOVE_CRL(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRL(BSP_GPIO_INPUT, i));
			}
			else
			{
				// GPIOx->CRH = (GPIOx->CRH & ~(BSP_GPIO_MOVE_CRH(BSP_GPIO_ALL_MASK, i))) | BSP_GPIO_MOVE_CRH(BSP_GPIO_INPUT, i);
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRH, BSP_GPIO_MOVE_CRH(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRH(BSP_GPIO_INPUT, i));
			}
		}
	}
}

void BSP_GPIO_set_pin_as_output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t i;
	for (i = 0x00; i < MAX_GPIO_INDEX; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			if(i < 8)
			{
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRL, BSP_GPIO_MOVE_CRL(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRL(BSP_GPIO_OUTPUT_PP, i));
			}
			else
			{
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRH, BSP_GPIO_MOVE_CRH(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRH(BSP_GPIO_OUTPUT_PP, i));
			}			
		}
	}
}

void BSP_GPIO_set_pin_as_analog(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t i;
	for (i = 0x00; i < MAX_GPIO_INDEX; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			if(i < 8)
			{
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRL, BSP_GPIO_MOVE_CRL(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRL(BSP_GPIO_ANALOG, i));
			}
			else
			{
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRH, BSP_GPIO_MOVE_CRH(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRH(BSP_GPIO_ANALOG, i));
			}
		}
	}
}

void BSP_GPIO_set_pin_as_alternate(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	uint8_t i;

	for (i = 0x0; i < MAX_GPIO_INDEX; i++)
	{
		if (GPIO_Pin & (1 << i))
		{
			if(i < 8)
			{
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRL, BSP_GPIO_MOVE_CRL(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRL(BSP_GPIO_AF_PP, i));
			}
			else
			{
				BSP_GPIO_SET_MODE_BITS(GPIOx->CRH, BSP_GPIO_MOVE_CRH(BSP_GPIO_ALL_MASK, i), BSP_GPIO_MOVE_CRH(BSP_GPIO_AF_PP, i));
			}
		}
	}
}
