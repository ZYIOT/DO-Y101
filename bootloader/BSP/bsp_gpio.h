#include "gpio.h"
#include "app.h"
#include "hardware_wrapper.h"

#define BSP_MBEDTLS_CHECK_STATUS()  (HARDWARE_GPIO_READ_PIN(MBEDTLS_CHECK_GPIO_Port, MBEDTLS_CHECK_Pin) == GPIO_PIN_RESET ? APP_OK : APP_ERROR)

