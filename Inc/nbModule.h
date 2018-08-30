
#ifndef __nbModule_H
#define __nbModule_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f0xx.h"

/*NB模块*/
#define IO_NB_OnOff GPIOB, LL_GPIO_PIN_15
#define IO_NB_Reset GPIOA, LL_GPIO_PIN_8

    extern UartDef UART_NB;
    extern void vNBHal_Init(void);

#endif
