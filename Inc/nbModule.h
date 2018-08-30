
#ifndef __nbModule_H
#define __nbModule_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f0xx.h"

/*运行灯*/
#define IO_RunLed GPIOB, LL_GPIO_PIN_8
/*FLASH*/
#define BUS_SPI SPI1
#define IO_Flash_CS GPIOA, LL_GPIO_PIN_4
#define IO_Flash_HOLD GPIOB, LL_GPIO_PIN_0
#define IO_Flash_WP GPIOB, LL_GPIO_PIN_1
/*温度芯片总线*/
#define BUS_OneWire GPIOB, LL_GPIO_PIN_12
/*NB模块*/
#define IO_NB_OnOff GPIOB, LL_GPIO_PIN_15
#define IO_NB_Reset GPIOA, LL_GPIO_PIN_8
/*串口驱动缓存*/
#define BUF_SIZE 0x1FF

#endif
