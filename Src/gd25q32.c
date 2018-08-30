/***********************************************************************
* Copyright (c) 2015,积成电子股份有限公司 All rights reserved.
*
* 文件名称： WindBond_Flash.c
* 描    述： Flash存取源文件
*
* 文件说明： Flash的移植只需要修改FlashInit()函数中引脚的配置和WindBond_Flash.h
             中的通信方式、引脚部分的宏配置即可。
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
* 函数名称: FlashInit
* 功能描述: Flash初始化
* 输入参数:
* 输出参数:
* 返 回 值:
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
* 函数名称: FlashSectorErase
* 功能描述: Flash指定扇区擦除.
* 输入参数: SectorAddr: 扇区地址.
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashBlockErase
* 功能描述: Flash指定块擦除.
* 输入参数: BlockAddr: 块地址.
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashBulkErase
* 功能描述: Flash全擦除
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashPageWrite
* 功能描述: Flash页数据写入，写入字节数不能超过页的大小
* 输入参数: pBuffer : 数据缓冲区首地址
            WriteAddr : Flash写入地址
            NumByteToWrite : 写入字节个数(不大于FLASH_PageSize  Flash页大小)
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashBufferWrite
* 功能描述: Flash块数据写入
* 输入参数: pBuffer : 数据缓冲区首地址
            WriteAddr : Flash写入地址
            NumByteToWrite : 写入字节个数
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashBufferRead
* 功能描述: Flash块数据读
* 输入参数: pBuffer : 数据缓冲区首地址
            WriteAddr : Flash读地址
            NumByteToWrite : 读出字节个数
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashIDRead
* 功能描述: Flash  ID读
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 读取的ID
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
* 函数名称: FlashDeviceIDRead
* 功能描述: Flash设备ID读
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 设备ID
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
* 函数名称: FlashStartReadSequence
* 功能描述: Flash按地址读取数据
* 输入参数: 读地址
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashByteRead
* 功能描述: Flash读取一个字节数据
            只有函数FlashStartReadSequence被调用后这个函数才能用，
            即该函数只被FlashStartReadSequence调用
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 读取的字节
****************************************************************/
    UINT8 FlashByteRead(void)
    {
        return (FlashByteSend(Dummy_Byte));
    }

    /***************************************************************
* 函数名称: FlashByteSend
* 功能描述: 向Flash输入一个字节数据(即写Flash)
* 输入参数: byte: 写入数据
* 输出参数: 无
* 返 回 值: 写结果
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
* 函数名称: FlashHalfWordSend
* 功能描述: 向Flash输入半个字数据(即写Flash)
* 输入参数: Half Word : 写入数据
* 输出参数: 无
* 返 回 值: 写结果
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
* 函数名称: FlashWriteEnable
* 功能描述: Flash写使能
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashWaitForWriteEnd
* 功能描述: Flash等待写完
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashPowerDown
* 功能描述: Flash掉电
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
* 函数名称: FlashWakeUp
* 功能描述: Flash唤醒
* 输入参数: 无
* 输出参数: 无
* 返 回 值: 无
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
