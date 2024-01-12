#ifndef __APP_H
#define __APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#define PI 3.14159265
#define APP_OK 0
#define APP_ERROR -1
#define APP_MALLOC_ERROR -2
#define APP_TIMEOUT -6
#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))
#define APP_CHECK_RC  \
    if (rc != APP_OK) \
    {                 \
        return rc;    \
    }
#define APP_CHECK_RC_AND_RESET \
    if (rc != APP_OK)          \
    {                          \
        BSP_MCU_reset();       \
    }

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "hardware_wrapper.h"


int app_main(void);

#ifdef __cplusplus
}
#endif

#endif
