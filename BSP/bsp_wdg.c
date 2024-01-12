
#include "hardware_wrapper.h"
#include "bsp_wdg.h"
#include "bsp_log.h"

#ifdef DISABLE_WDG
void BSP_WDG_init(void)
{
}
void BSP_WDG_feed(void)
{
}
void BSP_WDG_stop(void)
{
}
#else
static int sp706_wdi = GPIO_PIN_RESET;
static uint32_t last_tick = 0;
void BSP_WDG_init(void)
{
    MX_IWDG_Init();
}

void BSP_WDG_feed(void)
{
    uint32_t tick = HARDWARE_GET_TICK();
    if (tick - last_tick < 100)
    {
        return;
    }
    last_tick = tick;
    HAL_IWDG_Refresh(&hiwdg);
}

void BSP_WDG_stop(void)
{
    __HAL_IWDG_RELOAD_COUNTER(&hiwdg);
}
#endif
