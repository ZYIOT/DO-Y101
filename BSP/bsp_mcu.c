#include "bsp_mcu.h"


int BSP_MCU_reset(void)
{
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
    return APP_OK;
}
