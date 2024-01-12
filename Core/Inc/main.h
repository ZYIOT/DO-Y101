/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define WAVE_Pin GPIO_PIN_4
#define WAVE_GPIO_Port GPIOA
#define LED_BLUE_CTRL_Pin GPIO_PIN_2
#define LED_BLUE_CTRL_GPIO_Port GPIOA
#define ADC_WAVE_Pin GPIO_PIN_0
#define ADC_WAVE_GPIO_Port GPIOB
#define ADC_SIGNAL_Pin GPIO_PIN_1
#define ADC_SIGNAL_GPIO_Port GPIOB
#define LED_RED_CTRL_Pin GPIO_PIN_3
#define LED_RED_CTRL_GPIO_Port GPIOA
#define RS485_WRITE_Pin GPIO_PIN_7
#define RS485_WRITE_GPIO_Port GPIOC

#define IIC_SDA_Pin GPIO_PIN_15
#define IIC_SDA_GPIO_Port GPIOB
#define IIC_SCL_Pin GPIO_PIN_14
#define IIC_SCL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define SIZE_128B					(0x80)
#define SIZE_256B					(0x100)
#define SIZE_512B					(0x200)

#define GPIO_IIC_LIB            // 使用 GPIO 来模仿 HAL 库执行 IIC 操作 
#define SHOW_ADC_TEMPERATURE  // 显示从 ADC 到 TEMPERATURE 的数据 
// #define SHOW_IIC_DATA_DEBUG  // 显示 IIC 的数据 
// #define SHOW_MODBUS_DATA_DEBUG // 显示 MODBUS 收发的相关数据
// #define DISABLE_ADC_DEBUG 

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
