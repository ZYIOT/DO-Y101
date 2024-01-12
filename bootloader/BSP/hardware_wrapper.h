#ifndef __BSP_WRAPPER_H
#define __BSP_WRAPPER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "board_config.h"
#include "main.h"
#include "stdint.h"
#define HARDWARE_GPIO_WRITE_PIN(port, pin, state) HAL_GPIO_WritePin(port, pin, (state) > 0 ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define HARDWARE_GPIO_WRITE_PIN_H(port, pin) HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define HARDWARE_GPIO_WRITE_PIN_L(port, pin) HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define HARDWARE_GPIO_READ_PIN(port, pin) HAL_GPIO_ReadPin(port, pin)
#define HARDWARE_HAL_DELAY_MS(ms) HAL_Delay(ms)
#define HARDWARE_GET_TICK() HAL_GetTick()

#define HARDWARE_ENTER_CRITICAL()  __disable_irq()
#define HARDWARE_EXIT_CRITICAL()  __enable_irq()
#define HARDWARE_ENTER_CRITICAL_FROM_ISR()  HARDWARE_PortRaiseBASEPRI()
#define HARDWARE_EXIT_CRITICAL_FROM_ISR(x)   HARDWARE_PortSetBASEPRI(x)

#define HARDWARE_INTERRUPT_PRIORITY 50

    static __forceinline void HARDWARE_PortSetBASEPRI(uint32_t ulBASEPRI)
    {
        __asm
        {
		msr basepri, ulBASEPRI
        }
    }

    static __forceinline uint32_t HARDWARE_PortRaiseBASEPRI(void)
    {
        uint32_t ulReturn, ulNewBASEPRI = HARDWARE_INTERRUPT_PRIORITY;

        __asm
        {
		mrs ulReturn, basepri
		msr basepri, ulNewBASEPRI
		dsb
		isb
        }
        return ulReturn;
    }

#ifdef __cplusplus
}
#endif
#endif
