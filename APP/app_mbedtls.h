#ifndef __APP_MBEDTLS_H__
#define __APP_MBEDTLS_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "app.h"

    void APP_Flash_RDP_init(uint8_t status);
    void APP_MBEDTLS_task_run(void *argument);
    
#ifdef __cplusplus
}
#endif
#endif
