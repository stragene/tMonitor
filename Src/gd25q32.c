/***********************************************************************
* Copyright (c) 2015,���ɵ��ӹɷ����޹�˾ All rights reserved.
*
* �ļ����ƣ� WindBond_Flash.c
* ��    ���� Flash��ȡԴ�ļ�
*
* �ļ�˵���� Flash����ֲֻ��Ҫ�޸�FlashInit()���������ŵ����ú�WindBond_Flash.h
             �е�ͨ�ŷ�ʽ�����Ų��ֵĺ����ü��ɡ�
*
***********************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

//#include "Shell.h"
#include "WindBond_Flash.h"
#include "stm32f3xx.h"
#include "spi.h"

    /*----------------------------------------------------------------------
                            �ڲ���������
----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------
                            ȫ�ֱ�������
----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------
                          �ⲿȫ�ֱ�������
----------------------------------------------------------------------*/

    /*----------------------------------------------------------------------
                            �ڲ���������
----------------------------------------------------------------------*/

    /***************************************************************
* ��������: FlashInit
* ��������: Flash��ʼ��
* �������:
* �������:
* �� �� ֵ:
****************************************************************/
    void FlashInit(void)
    {
        HAL_SPI_MspInit(hspi1);
        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();

        //SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);

        /* Enable SPI1  */
        //SPI_Cmd(SPI1, ENABLE);
    }

    /***************************************************************
* ��������: FlashSectorErase
* ��������: Flashָ����������.
* �������: SectorAddr: ������ַ.
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashSectorErase(UINT32 SectorAddr)
    {
        /* Send write enable instruction */
        FlashWriteEnable();
        FlashWaitForWriteEnd();
        /* Sector Erase */
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();
        /* Send Sector Erase instruction */
        FlashByteSend(W25X_SectorErase);
        /* Send SectorAddr high nibble address byte */
        FlashByteSend((SectorAddr & 0xFF0000) >> 16);
        /* Send SectorAddr medium nibble address byte */
        FlashByteSend((SectorAddr & 0xFF00) >> 8);
        /* Send SectorAddr low nibble address byte */
        FlashByteSend(SectorAddr & 0xFF);
        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
        /* Wait the end of Flash writing */
        FlashWaitForWriteEnd();
    }

    /***************************************************************
* ��������: FlashBlockErase
* ��������: Flashָ�������.
* �������: BlockAddr: ���ַ.
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashBlockErase(UINT32 BlockAddr)
    {
        /* Send write enable instruction */
        FlashWriteEnable();
        FlashWaitForWriteEnd();
        /* Block Erase */
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();
        /* Send Block Erase instruction */
        FlashByteSend(W25X_BlockErase);
        /* Send BlockAddr high nibble address byte */
        FlashByteSend((BlockAddr & 0xFF0000) >> 16);
        /* Send BlockAddr medium nibble address byte */
        FlashByteSend((BlockAddr & 0xFF00) >> 8);
        /* Send BlockAddr low nibble address byte */
        FlashByteSend(BlockAddr & 0xFF);
        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
        /* Wait the end of Flash writing */
        FlashWaitForWriteEnd();
    }

    /***************************************************************
* ��������: FlashBulkErase
* ��������: Flashȫ����
* �������: ��
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashBulkErase(void)
    {
        /* Send write enable instruction */
        FlashWriteEnable();
        FlashWaitForWriteEnd();

        /* Bulk Erase */
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();
        /* Send Bulk Erase instruction  */
        FlashByteSend(W25X_ChipErase);
        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();

        /* Wait the end of Flash writing */
        FlashWaitForWriteEnd();
    }

    /***************************************************************
* ��������: FlashPageWrite
* ��������: Flashҳ����д�룬д���ֽ������ܳ���ҳ�Ĵ�С
* �������: pBuffer : ���ݻ������׵�ַ
            WriteAddr : Flashд���ַ
            NumByteToWrite : д���ֽڸ���(������FLASH_PageSize  Flashҳ��С)
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashPageWrite(UINT8 *pBuffer, UINT32 WriteAddr, UINT16 NumByteToWrite)
    {
        /* Enable the write access to the FLASH */
        FlashWriteEnable();
        FlashWaitForWriteEnd();

        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();
        /* Send "Write to Memory " instruction */
        FlashByteSend(W25X_PageProgram);
        /* Send WriteAddr high nibble address byte to write to */
        FlashByteSend((WriteAddr & 0xFF0000) >> 16);
        /* Send WriteAddr medium nibble address byte to write to */
        FlashByteSend((WriteAddr & 0xFF00) >> 8);
        /* Send WriteAddr low nibble address byte to write to */
        FlashByteSend(WriteAddr & 0xFF);

        if (NumByteToWrite > PAGE_SIZE)
        {
            NumByteToWrite = PAGE_SIZE;
            //printf("\n\r Err: FlashPageWrite too large!");
        }

        /* while there is data to be written on the FLASH */
        while (NumByteToWrite--)
        {
            /* Send the current byte */
            FlashByteSend(*pBuffer);
            /* Point on the next byte to be written */
            pBuffer++;
        }

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();

        /* Wait the end of Flash writing */
        FlashWaitForWriteEnd();
    }

    /***************************************************************
* ��������: FlashBufferWrite
* ��������: Flash������д��
* �������: pBuffer : ���ݻ������׵�ַ
            WriteAddr : Flashд���ַ
            NumByteToWrite : д���ֽڸ���
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashBufferWrite(UINT8 *pBuffer, UINT32 WriteAddr, UINT16 NumByteToWrite)
    {
        UINT8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

        Addr = WriteAddr % PAGE_SIZE;
        count = PAGE_SIZE - Addr;
        NumOfPage = NumByteToWrite / PAGE_SIZE;
        NumOfSingle = NumByteToWrite % PAGE_SIZE;

        if (Addr == 0) /* WriteAddr is PAGE_SIZE aligned  */
        {
            if (NumOfPage == 0) /* NumByteToWrite < PAGE_SIZE */
            {
                FlashPageWrite(pBuffer, WriteAddr, NumByteToWrite);
            }
            else /* NumByteToWrite > PAGE_SIZE */
            {
                while (NumOfPage--)
                {
                    FlashPageWrite(pBuffer, WriteAddr, PAGE_SIZE);
                    WriteAddr += PAGE_SIZE;
                    pBuffer += PAGE_SIZE;
                }

                FlashPageWrite(pBuffer, WriteAddr, NumOfSingle);
            }
        }
        else /* WriteAddr is not PAGE_SIZE aligned  */
        {
            if (NumOfPage == 0) /* NumByteToWrite < PAGE_SIZE */
            {
                if (NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > PAGE_SIZE */
                {
                    temp = NumOfSingle - count;

                    FlashPageWrite(pBuffer, WriteAddr, count);
                    WriteAddr += count;
                    pBuffer += count;

                    FlashPageWrite(pBuffer, WriteAddr, temp);
                }
                else
                {
                    FlashPageWrite(pBuffer, WriteAddr, NumByteToWrite);
                }
            }
            else /* NumByteToWrite > PAGE_SIZE */
            {
                NumByteToWrite -= count;
                NumOfPage = NumByteToWrite / PAGE_SIZE;
                NumOfSingle = NumByteToWrite % PAGE_SIZE;

                FlashPageWrite(pBuffer, WriteAddr, count);
                WriteAddr += count;
                pBuffer += count;

                while (NumOfPage--)
                {
                    FlashPageWrite(pBuffer, WriteAddr, PAGE_SIZE);
                    WriteAddr += PAGE_SIZE;
                    pBuffer += PAGE_SIZE;
                }

                if (NumOfSingle != 0)
                {
                    FlashPageWrite(pBuffer, WriteAddr, NumOfSingle);
                }
            }
        }
    }

    /***************************************************************
* ��������: FlashBufferRead
* ��������: Flash�����ݶ�
* �������: pBuffer : ���ݻ������׵�ַ
            WriteAddr : Flash����ַ
            NumByteToWrite : �����ֽڸ���
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashBufferRead(UINT8 *pBuffer, UINT32 ReadAddr, UINT16 NumByteToRead)
    {
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "Read from Memory " instruction */
        FlashByteSend(W25X_ReadData);

        /* Send ReadAddr high nibble address byte to read from */
        FlashByteSend((ReadAddr & 0xFF0000) >> 16);
        /* Send ReadAddr medium nibble address byte to read from */
        FlashByteSend((ReadAddr & 0xFF00) >> 8);
        /* Send ReadAddr low nibble address byte to read from */
        FlashByteSend(ReadAddr & 0xFF);

        while (NumByteToRead--) /* while there is data to be read */
        {
            /* Read a byte from the FLASH */
            *pBuffer = FlashByteSend(Dummy_Byte);
            /* Point to the next location where the byte read will be saved */
            pBuffer++;
        }

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
    }

    /***************************************************************
* ��������: FlashIDRead
* ��������: Flash  ID��
* �������: ��
* �������: ��
* �� �� ֵ: ��ȡ��ID
****************************************************************/
    UINT32 FlashIDRead(void)
    {
        UINT32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "RDID " instruction */
        FlashByteSend(W25X_JedecDeviceID);

        /* Read a byte from the FLASH */
        Temp0 = FlashByteSend(Dummy_Byte);

        /* Read a byte from the FLASH */
        Temp1 = FlashByteSend(Dummy_Byte);

        /* Read a byte from the FLASH */
        Temp2 = FlashByteSend(Dummy_Byte);

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();

        Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

        return Temp;
    }

    /***************************************************************
* ��������: FlashDeviceIDRead
* ��������: Flash�豸ID��
* �������: ��
* �������: ��
* �� �� ֵ: �豸ID
****************************************************************/
    UINT32 FlashDeviceIDRead(void)
    {
        UINT32 Temp = 0;

        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "RDID " instruction */
        FlashByteSend(W25X_DeviceID);
        FlashByteSend(Dummy_Byte);
        FlashByteSend(Dummy_Byte);
        FlashByteSend(Dummy_Byte);

        /* Read a byte from the FLASH */
        Temp = FlashByteSend(Dummy_Byte);

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();

        return Temp;
    }

    /***************************************************************
* ��������: FlashStartReadSequence
* ��������: Flash����ַ��ȡ����
* �������: ����ַ
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashStartReadSequence(UINT32 ReadAddr)
    {
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "Read from Memory " instruction */
        FlashByteSend(W25X_ReadData);

        /* Send the 24-bit address of the address to read from */
        /* Send ReadAddr high nibble address byte */
        FlashByteSend((ReadAddr & 0xFF0000) >> 16);
        /* Send ReadAddr medium nibble address byte */
        FlashByteSend((ReadAddr & 0xFF00) >> 8);
        /* Send ReadAddr low nibble address byte */
        FlashByteSend(ReadAddr & 0xFF);
    }

    /***************************************************************
* ��������: FlashByteRead
* ��������: Flash��ȡһ���ֽ�����
            ֻ�к���FlashStartReadSequence�����ú�������������ã�
            ���ú���ֻ��FlashStartReadSequence����
* �������: ��
* �������: ��
* �� �� ֵ: ��ȡ���ֽ�
****************************************************************/
    UINT8 FlashByteRead(void)
    {
        return (FlashByteSend(Dummy_Byte));
    }

    /***************************************************************
* ��������: FlashByteSend
* ��������: ��Flash����һ���ֽ�����(��дFlash)
* �������: byte: д������
* �������: ��
* �� �� ֵ: д���
****************************************************************/
    UINT8 FlashByteSend(UINT8 byte)
    {
        UINT32 udwCount = 0;

        if (ucPressureTestFlag == 1)
        {
            while (1)
            {
            }
        }

        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        {
#if 0
        if (udwCount++ < 5000)
        {
            ;
        }
        else
        {
            break;
        }
#endif
        }

        /* Send byte through the SPI1 peripheral */
        SPI_SendData8(SPI1, byte);

        /* Wait to receive a byte */
        udwCount = 0;
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        {
#if 0
        if (udwCount++ < 5000)
        {
            ;
        }
        else
        {
            break;
        }
#endif
        }

        /* Return the byte read from the SPI bus */
        return SPI_ReceiveData8(SPI1);
    }

    /***************************************************************
* ��������: FlashHalfWordSend
* ��������: ��Flash������������(��дFlash)
* �������: Half Word : д������
* �������: ��
* �� �� ֵ: д���
****************************************************************/
    UINT16 FlashHalfWordSend(UINT16 HalfWord)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
            ;

        /* Send Half Word through the SPI1 peripheral */
        SPI_SendData8(SPI1, HalfWord);

        /* Wait to receive a Half Word */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
            ;

        /* Return the Half Word read from the SPI bus */
        return SPI_ReceiveData8(SPI1);
    }

    /***************************************************************
* ��������: FlashWriteEnable
* ��������: Flashдʹ��
* �������: ��
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashWriteEnable(void)
    {
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "Write Enable" instruction */
        FlashByteSend(W25X_WriteEnable);

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
    }

    /***************************************************************
* ��������: FlashWaitForWriteEnd
* ��������: Flash�ȴ�д��
* �������: ��
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashWaitForWriteEnd(void)
    {
        UINT8 FLASH_Status = 0;

        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "Read Status Register" instruction */
        FlashByteSend(W25X_ReadStatusReg);

        /* Loop as long as the memory is busy with a write cycle */
        do
        {
            /* Send a dummy byte to generate the clock needed by the FLASH
        and put the value of the status register in FLASH_Status variable */
            FLASH_Status = FlashByteSend(Dummy_Byte);
        } while ((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
    }

    /***************************************************************
* ��������: FlashPowerDown
* ��������: Flash����
* �������: ��
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashPowerDown(void)
    {
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "Power Down" instruction */
        /* FlashByteSend(W25X_PowerDown) */
        FlashByteSend(W25X_ReleasePowerDown);
        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
    }

    /***************************************************************
* ��������: FlashWakeUp
* ��������: Flash����
* �������: ��
* �������: ��
* �� �� ֵ: ��
****************************************************************/
    void FlashWakeUp(void)
    {
        /* Select the FLASH: Chip Select low */
        FLASH_CS_LOW();

        /* Send "Power Down" instruction */
        FlashByteSend(W25X_ReleasePowerDown);

        /* Deselect the FLASH: Chip Select high */
        FLASH_CS_HIGH();
    }

#ifdef __cplusplus
}
#endif
