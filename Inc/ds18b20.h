#ifndef __ds18b20_H_
#define __ds18b20_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f0xx.h"

/*温度芯片总线*/
#define PORT_OneWire GPIOB
#define PIN_OneWire LL_GPIO_PIN_12

    typedef struct OneWire_TypeDef
    {
        GPIO_TypeDef *GPIOx;
        uint16_t GPIO_Pin;
        bool (*Initialize)(struct OneWire_TypeDef *OneWire);
        void (*Read)(struct OneWire_TypeDef *OneWire, uint8_t length, uint8_t *Data);
        void (*WriteByte)(struct OneWire_TypeDef *OneWire, uint8_t data);
        void (*_Delay)(__IO uint32_t nTime);
        uint8_t (*_CRC8)(uint8_t crc, uint8_t data);

    } OneWire_TypeDef;

    void OneWireStructInit(OneWire_TypeDef *OneWire);
    bool DS18D20_ReadTemperature(OneWire_TypeDef *OneWire, uint8_t *Temperature);
    //void OneWire_Delay(__IO uint32_t nTime);

#ifdef __cplusplus
}
#endif

#endif /* DS18B20_H_ */
