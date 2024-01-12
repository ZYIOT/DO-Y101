#ifndef __BSP_LED_H
#define __BSP_LED_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "app.h"


#define BSP_LED_BLUE_ON() HARDWARE_GPIO_WRITE_PIN_H(LED_BLUE_CTRL_GPIO_Port, LED_BLUE_CTRL_Pin)
#define BSP_LED_BLUE_OFF() HARDWARE_GPIO_WRITE_PIN_L(LED_BLUE_CTRL_GPIO_Port, LED_BLUE_CTRL_Pin)

#define BSP_LED_RED_ON() HARDWARE_GPIO_WRITE_PIN_H(LED_RED_CTRL_GPIO_Port, LED_RED_CTRL_Pin)
#define BSP_LED_RED_OFF() HARDWARE_GPIO_WRITE_PIN_L(LED_RED_CTRL_GPIO_Port, LED_RED_CTRL_Pin)

#ifdef __cplusplus
}
#endif

#endif
