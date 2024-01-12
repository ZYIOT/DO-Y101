#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "bsp_usart.h"
#include "bsp_log.h"
#include "app.h"
#include "app_task.h"
#include "app_adc.h"
#include "app_temperature.h"
#include "app_modbus.h"
#include "app_mbedtls.h"
#include "main.h"

#define __RUN_TASK(_name, _priority, _stack)                                                      \
    const osThreadAttr_t _name##_task_attributes = {                                              \
        .name = #_name "Task", .priority = (osPriority_t)_priority, .stack_size = _stack};        \
    _##_name##_task_handle = osThreadNew(APP_##_name##_task_run, NULL, &_name##_task_attributes); \
    BSP_LOG_debug(#_name " handle %x\r\n", _##_name##_task_handle);                               \
    HAL_Delay(10);

#define __DEF_TASK(_name) \
    osThreadId_t _##_name##_task_handle;

// __DEF_TASK(CONFIG)

__DEF_TASK(ADC)
__DEF_TASK(TEMPERATURE)
__DEF_TASK(MODBUS)
#ifdef ENABLE_MBEDTLS_MD5
__DEF_TASK(MBEDTLS)
#endif

int APP_TASK_init(void)
{
    osKernelInitialize();
    // __RUN_TASK(CONFIG, osPriorityNormal, 1536);
    __RUN_TASK(ADC, osPriorityHigh, 0x3000);
    __RUN_TASK(TEMPERATURE, osPriorityHigh, 0x1000);
    __RUN_TASK(MODBUS, osPriorityNormal, 0x2000);
#ifdef ENABLE_MBEDTLS_MD5
    __RUN_TASK(MBEDTLS, osPriorityNormal, 0x2000);
#endif    
    return APP_OK;
}

void APP_TASK_run(void)
{
    osKernelStart();
    while (1)
    {
    }
}
