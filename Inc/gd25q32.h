/***********************************************************************
* Copyright (c) 2015,���ɵ��ӹɷ����޹�˾ All rights reserved.
*
* �ļ����ƣ� gd25q32.h
* ��    ���� Flash��ȡͷ�ļ�
*
* �ļ�˵����
*
***********************************************************************/

#ifndef __gd25q32_H
#define __gd25q32_H

#include "stm32f0xx.h"
//#include "stm32f0xx_ll_gpio.h"

/*----------------------------------------------------------------------
                                �궨��
----------------------------------------------------------------------*/
/*---------------------------ID��ҳ��С��ָ��---------------------------*/

/*Flash ID*/
#define FLASH_ID 0xC84015 /*Flash��һ��ʹ��Ҫ���ж�ID*/

/*Flash ҳ��С*/
#define PAGE_SIZE 256

/* Flash ����ָ��궨�� */
#define W25X_WriteEnable 0x06      /*дʹ��*/
#define W25X_WriteDisable 0x04     /*д��ֹ*/
#define W25X_ReadStatusReg 0x05    /*������״̬*/
#define W25X_WriteStatusReg 0x01   /*д����״̬*/
#define W25X_ReadData 0x03         /*������*/
#define W25X_FastReadData 0x0B     /*���ٶ�����*/
#define W25X_FastReadDual 0x3B     /*���߿��ٶ�����*/
#define W25X_PageProgram 0x02      /*ҳ���*/
#define W25X_BlockErase 0xD8       /*�����, ÿ�β�64KB*/
#define W25X_SectorErase 0x20      /*��������, ÿ�β�4KB*/
#define W25X_ChipErase 0xC7        /*����оƬ����*/
#define W25X_PowerDown 0xB9        /*����*/
#define W25X_ReleasePowerDown 0xAB /*����*/
#define W25X_DeviceID 0xAB         /*Flash ID*/
#define W25X_ManufactDeviceID 0x90 /*������ ID*/
#define W25X_JedecDeviceID 0x9F    /*Jedece��׼�� Flash ID*/

/* Write In Progress����д��־ */
#define WIP_Flag 0x01

/*��������λָ�ʹFlash��������ģʽ��λ����ͨģʽ��һ������ϵͳ��λ��λFlash*/
#define Dummy_Byte 0xFF

/*---------------------------ͨ�ŷ�ʽ�����š�ʹ��/��ֹ---------------------------*/
//#define FLASH_SPI SPI1 /*����SPI�ӿ�*/
//#define FLASH_CLK RCC_APB2Periph_SPI1
//
///*SCK ��������*/
//#define FLASH_SCK_PIN GPIO_Pin_5  /* PA.05 */
//#define FLASH_SCK_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_SCK_GPIO_CLK RCC_AHBPeriph_GPIOA
//#define FLASH_SCK_SOURCE GPIO_PinSource5
//#define FLASH_SCK_AF GPIO_AF_5
//
///*MISO ��������*/
//#define FLASH_MISO_PIN GPIO_Pin_6  /* PA.06 */
//#define FLASH_MISO_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_MISO_GPIO_CLK RCC_AHBPeriph_GPIOA
//#define FLASH_MISO_SOURCE GPIO_PinSource6
//#define FLASH_MISO_AF GPIO_AF_5
//
///*MOSI ��������*/
//#define FLASH_MOSI_PIN GPIO_Pin_7  /* PA.07 */
//#define FLASH_MOSI_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_MOSI_GPIO_CLK RCC_AHBPeriph_GPIOA
//#define FLASH_MOSI_SOURCE GPIO_PinSource7
//#define FLASH_MOSI_AF GPIO_AF_5
//
///*CS ��������*/
//#define FLASH_CS_PIN GPIO_Pin_0  /* PA.00 */
//#define FLASH_CS_GPIO_PORT GPIOA /* GPIOA */
//#define FLASH_CS_GPIO_CLK RCC_AHBPeriph_GPIOA

/*ʹ��/��ֹ ����*/
#define FLASH_CS_LOW() LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_4)
#define FLASH_CS_HIGH() LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_4)
/*д����ʹ��/���� ����*/
#define FLASH_WP_EN() LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_1)
#define FLASH_WP_DEN() LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_1)

/*----------------------------------------------------------------------
                           ö�١��ṹ�嶨��
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
                               ��������
----------------------------------------------------------------------*/
void FlashInit(void);
void FlashSectorErase(uint32_t SectorAddr);
void FlashBlockErase(uint32_t BlockAddr);
void FlashBulkErase(void);
void FlashPageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void FlashBufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void FlashBufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
uint32_t FlashIDRead(void);
uint32_t FlashDeviceIDRead(void);
void FlashStartReadSequence(uint32_t ReadAddr);
void FlashPowerDown(void);
void FlashWakeUp(void);
uint8_t FlashByteRead(void);
uint8_t FlashByteSend(uint8_t byte);
uint16_t FlashHalfWordSend(uint16_t HalfWord);
void FlashWriteEnable(void);
void FlashWaitForWriteEnd(void);

#endif
