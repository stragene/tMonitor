
#ifndef __ds18b20_H
#define __ds18b20_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f0xx.h"

/*温度芯片总线*/
#define BUS_OneWire GPIOB, LL_GPIO_PIN_12

#endif
