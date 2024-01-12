#ifndef __BSP_DAC_H
#define __BSP_DAC_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "app.h"
#include "dac.h"
#include "tim.h"
    int BSP_DAC_init(void);
    int BSP_DAC_start(void);
    int BSP_DAC_stop(void);
#ifdef __cplusplus
}
#endif

#endif
