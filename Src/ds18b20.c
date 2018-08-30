
#include "ds18b20.h"
/*----------------------------------------------------------------------
                            内部函数声明
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
                            全局变量定义
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
                          外部全局变量声明
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
                            内部函数定义
----------------------------------------------------------------------*/

/***************************************************************
* 函数名称: OneWire总线初始化
* 功能描述: Pull line --> wait for 490 us --> 
*          release line --> wait for 60 us --> 
*          check if line is pulled down
* 输入参数: 
* 输出参数: 
* 返 回 值: 
****************************************************************/
bool _OneWire_Initialize(OneWire_TypeDef *OneWire)
{
    // Initialization sequence
    OneWire->GPIOx->BRR = OneWire->GPIO_Pin;
    OneWire->_Delay(480);
    OneWire->GPIOx->BSRR = OneWire->GPIO_Pin;
    OneWire->_Delay(65);

    if ((OneWire->GPIOx->IDR & OneWire->GPIO_Pin) != 0x00)
    {
        // no one-wire devices found
        return false;
    }
    OneWire->_Delay(405);

    return true;
}

/***************************************************************
* 函数名称: _OneWire_WriteByte发送数据
* 功能描述: 
*         发1：pull line --> wait for 1 us --> release line        
*         发0：pull line --> wait for 90 us --> release line        
* 输入参数: 
* 输出参数: 
* 返 回 值: 
****************************************************************/
void _OneWire_WriteByte(OneWire_TypeDef *OneWire, uint8_t data)
{
    for (uint8_t i = 0x00; i < 0x08; i++)
    {
        if ((data & 0x01) == 0x01)
        {
            // Send 1
            OneWire->GPIOx->BRR = OneWire->GPIO_Pin;
            OneWire->_Delay(1);
            OneWire->GPIOx->BSRR = OneWire->GPIO_Pin;
            OneWire->_Delay(60);
        }
        else
        {
            // Send "0"
            OneWire->GPIOx->BRR = OneWire->GPIO_Pin;
            OneWire->_Delay(60);
            OneWire->GPIOx->BSRR = OneWire->GPIO_Pin;
            OneWire->_Delay(10);
        }

        data = data >> 1;
    }
}

/***************************************************************
* 函数名称: _OneWire_Read读取数据
* 功能描述: 
*         pull line --> wait for 2 us --> release line --> 
          wait for 20 us --> read value(ODR?) --> wait for 45us
* 输入参数: length：读取长度;   Data：存储位置
* 输出参数: 
* 返 回 值: 
****************************************************************/
void _OneWire_Read(OneWire_TypeDef *OneWire, uint8_t length, uint8_t *Data)
{
    uint8_t bit = 0x00, byte = 0x00;
    for (byte = 0x00; byte < length; byte++)
    {
        Data[byte] = 0x00;
        for (bit = 0x00; bit < 0x08; bit++)
        {
            OneWire->GPIOx->BRR = OneWire->GPIO_Pin;
            OneWire->_Delay(2);
            OneWire->GPIOx->BSRR = OneWire->GPIO_Pin;
            OneWire->_Delay(20);
            if ((OneWire->GPIOx->ODR & OneWire->GPIO_Pin) != 0x00)
            {
                Data[byte] ^= 1 << bit;
            }
            OneWire->_Delay(45);
        }
    }
}
/***************************************************************
* 函数名称: _OneWire_CRC8 CRC校验
* 功能描述: 
* 输入参数: 
* 输出参数: 
* 返 回 值: 
****************************************************************/

uint8_t _OneWire_CRC8(uint8_t crc, uint8_t data)
{
    uint8_t i;

    crc = crc ^ data;
    for (i = 0; i < 8; i++)
    {
        if (crc & 0x01)
        {
            crc = (crc >> 1) ^ 0x8C;
        }
        else
        {
            crc >>= 1;
        }
    }

    return crc;
}

/***************************************************************
* 函数名称: DS18D20_ReadTemperature 
* 功能描述: 读取温度
* 输入参数: 
* 输出参数: 
* 返 回 值: 
****************************************************************/
bool DS18D20_ReadTemperature(OneWire_TypeDef *OneWire, uint8_t *Temperature)
{
    Temperature = 0x00;
    if (!OneWire->Initialize(OneWire))
    {
        return false;
    }

    OneWire->WriteByte(OneWire, 0xCC);
    OneWire->WriteByte(OneWire, 0x44);
    OneWire->GPIOx->BRR = OneWire->GPIO_Pin;
    OneWire->_Delay(755);
    OneWire->GPIOx->BSRR = OneWire->GPIO_Pin;
    if (!OneWire->Initialize(OneWire))
    {
        return false;
    }
    OneWire->WriteByte(OneWire, 0xCC);
    OneWire->WriteByte(OneWire, 0xBE);
    uint8_t Data[9] = {0x00};
    OneWire->Read(OneWire, 9, Data);
    uint8_t crc = 0x00;
    for (uint8_t i = 0x00; i < 9; i++)
    {
        crc = OneWire->_CRC8(crc, Data[i]);
    }

    if (crc != Data[8])
    {
        return false;
    }
    Temperature[0] = Data[0];
    Temperature[1] = Data[1];

    return true;
}

/***************************************************************
* 函数名称: OneWireStructInit
* 功能描述: 读取温度
* 输入参数: 
* 输出参数: 
* 返 回 值: 
****************************************************************/
void OneWireStructInit(OneWire_TypeDef *OneWire)
{
    OneWire->Initialize = &_OneWire_Initialize;
    OneWire->Read = &_OneWire_Read;
    OneWire->WriteByte = &_OneWire_WriteByte;
    /*OneWire->_Delay = &OneWire_Delay;*/
    /*LL库自带延时函数，需更改为us延时*/
    OneWire->_Delay = &LL_mDelay;
    OneWire->_CRC8 = &_OneWire_CRC8;
}
