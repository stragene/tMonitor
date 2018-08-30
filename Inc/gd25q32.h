/***********************************************************************
* Copyright (c) 2015,���ɵ��ӹɷ����޹�˾ All rights reserved.
*
* �ļ����ƣ� WindBond_Flash.h
* ��    ���� Flash��ȡͷ�ļ�
*
* �ļ�˵����
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
                                �궨��
----------------------------------------------------------------------*/
/*---------------------------ID��ҳ��С��ָ��---------------------------*/
/*Flash ID*/
#define FLASH_ID 0xC84017 /*Flash��һ��ʹ��Ҫ���ж�ID*/

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
#define FLASH_CS_LOW() HAL_GPIO_WritePin(GPIOA, GPIO_Pin_4, GPIO_PIN_RESET)
#define FLASH_CS_HIGH() HAL_GPIO_WritePin(GPIOA, GPIO_Pin_4, GPIO_PIN_SET)

    /*----------------------------------------------------------------------
                           ö�١��ṹ�嶨��
----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------
                               ��������
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
