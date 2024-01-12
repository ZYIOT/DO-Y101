#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "gpio.h"
#define MAX_GPIO_INDEX          (0x10)

#define CR_MODEy_INPUT          (0)
#define CR_MODEy_OUTPUT_LOW     (1)
#define CR_MODEy_OUTPUT_MEDIUM  (2)
#define CR_MODEy_OUTPUT_HIGH    (3)
// CR_MODEy_INPUT ʱ
#define CR_CNFy_ANALOG          (0<<2)
#define CR_CNFy_FLOAT           (1<<2)
#define CR_CNFy_DOWN_UP         (2<<2)
// Other ʱ
#define CR_CNFy_PP              (0<<2)
#define CR_CNFy_OD              (1<<2)
#define CR_CNFy_AF_PP           (2<<2)
#define CR_CNFy_AF_OD           (3<<2)

#define BSP_GPIO_INPUT      (CR_MODEy_INPUT + CR_CNFy_DOWN_UP)
#define BSP_GPIO_OUTPUT_PP  (CR_MODEy_OUTPUT_HIGH + CR_CNFy_PP)
#define BSP_GPIO_OUTPUT_OD  (CR_MODEy_OUTPUT_HIGH + CR_CNFy_OD)
#define BSP_GPIO_ANALOG     (CR_MODEy_INPUT + CR_CNFy_ANALOG)
#define BSP_GPIO_AF_PP      (CR_MODEy_OUTPUT_HIGH + CR_CNFy_AF_PP)
#define BSP_GPIO_AF_OD      (CR_MODEy_OUTPUT_HIGH + CR_CNFy_AF_OD)
#define BSP_GPIO_ALL_MASK   (3 + 3<<2)

#define BSP_GPIO_MOVE_CRL(val, move)    (val << (4 * move))
#define BSP_GPIO_MOVE_CRH(val, move)    (val << (4 * (move - 8)))
#define BSP_GPIO_SET_MODE_BITS(old, maskbits, setbits)    (old = (old & ~(maskbits)) | setbits)

    void BSP_GPIO_set_pin_as_input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
    void BSP_GPIO_set_pin_as_output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
    void BSP_GPIO_set_pin_as_analog(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
    void BSP_GPIO_set_pin_as_alternate(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif
#endif
