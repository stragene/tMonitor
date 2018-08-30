/***********************************************************************
* Copyright (c) 2015,积成电子股份有限公司 All rights reserved.
*
* 文件名称： WindBond_Flash.h
* 描    述： Flash存取头文件
*
* 文件说明：
*
***********************************************************************/

#ifndef WINDBOND_FLASH_H
#define WINDBOND_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f3xx.h"

/*----------------------------------------------------------------------
                                宏定义
----------------------------------------------------------------------*/
/*---------------------------ID、页大小、指令---------------------------*/
/*Flash ID*/
#define FLASH_ID 0xC84017 /*Flash第一次使用要先判断ID*/

/*Flash 页大小*/
#define PAGE_SIZE 256

/* Flash 操作指令宏定义 */
#define W25X_WriteEnable 0x06      /*写使能*/
#define W25X_WriteDisable 0x04     /*写禁止*/
#define W25X_ReadStatusReg 0x05    /*读数据状态*/
#define W25X_WriteStatusReg 0x01   /*写数据状态*/
#define W25X_ReadData 0x03         /*读数据*/
#define W25X_FastReadData 0x0B     /*快速读数据*/
#define W25X_FastReadDual 0x3B     /*两线快速读数据*/
#define W25X_PageProgram 0x02      /*页编程*/
#define W25X_BlockErase 0xD8       /*块擦除, 每次擦64KB*/
#define W25X_SectorErase 0x20      /*扇区擦除, 每次擦4KB*/
#define W25X_ChipErase 0xC7        /*整个芯片擦除*/
#define W25X_PowerDown 0xB9        /*掉电*/
#define W25X_ReleasePowerDown 0xAB /*唤醒*/
#define W25X_DeviceID 0xAB         /*Flash ID*/
#define W25X_ManufactDeviceID 0x90 /*制造商 ID*/
#define W25X_JedecDeviceID 0x9F    /*Jedece标准下 Flash ID*/

/* Write In Progress正在写标志 */
#define WIP_Flag 0x01

/*连续读复位指令，使Flash从连续读模式复位成普通模式，一般用于系统复位后复位Flash*/
#define Dummy_Byte 0xFF

/*---------------------------通信方式、引脚、使能/禁止---------------------------*/
//#define FLASH_SPI SPI1 /*采用SPI接口*/
//#define FLASH_CLK RCC_APB2Periph_SPI1
//
///*SCK 引脚配置*/
//#define FLASH_SCK_PIN GPIO_Pin_5  /* PA.05 */
//#define FLASH_SCK_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_SCK_GPIO_CLK RCC_AHBPeriph_GPIOA
//#define FLASH_SCK_SOURCE GPIO_PinSource5
//#define FLASH_SCK_AF GPIO_AF_5
//
///*MISO 引脚配置*/
//#define FLASH_MISO_PIN GPIO_Pin_6  /* PA.06 */
//#define FLASH_MISO_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_MISO_GPIO_CLK RCC_AHBPeriph_GPIOA
//#define FLASH_MISO_SOURCE GPIO_PinSource6
//#define FLASH_MISO_AF GPIO_AF_5
//
///*MOSI 引脚配置*/
//#define FLASH_MOSI_PIN GPIO_Pin_7  /* PA.07 */
//#define FLASH_MOSI_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_MOSI_GPIO_CLK RCC_AHBPeriph_GPIOA
//#define FLASH_MOSI_SOURCE GPIO_PinSource7
//#define FLASH_MOSI_AF GPIO_AF_5
//
///*CS 引脚配置*/
//#define FLASH_CS_PIN GPIO_Pin_0  /* PA.00 */
//#define FLASH_CS_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_CS_GPIO_CLK RCC_AHBPeriph_GPIOA

/*使能/禁止 配置*/
#define FLASH_CS_LOW() HAL_GPIO_WritePin(GPIOA, GPIO_Pin_4, GPIO_PIN_RESET)
#define FLASH_CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_Pin_4, GPIO_PIN_SET)

    /*----------------------------------------------------------------------
                           枚举、结构体定义
----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------
                               函数声明
----------------------------------------------------------------------*/
    void FlashInit(void);
    void FlashSectorErase(u32 SectorAddr);
    void FlashBlockErase(UINT32 BlockAddr);
    void FlashBulkErase(void);
    void FlashPageWrite(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);
    void FlashBufferWrite(u8 *pBuffer, u32 WriteAddr, u16 NumByteToWrite);
    void FlashBufferRead(u8 *pBuffer, u32 ReadAddr, u16 NumByteToRead);
    UINT32 FlashIDRead(void);
    UINT32 FlashDeviceIDRead(void);
    void FlashStartReadSequence(u32 ReadAddr);
    void FlashPowerDown(void);
    void FlashWakeUp(void);
    UINT8 FlashByteRead(void);
    UINT8 FlashByteSend(u8 byte);
    UINT16 FlashHalfWordSend(u16 HalfWord);
    void FlashWriteEnable(void);
    void FlashWaitForWriteEnd(void);

#ifdef __cplusplus
}
#endif

#endif
