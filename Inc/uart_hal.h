
#ifndef __uart_hal_H
#define __uart_hal_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f0xx.h"

/*串口驱动缓存*/
#define BUF_SIZE 0x1FF

    struct ucbuf
    {
        uint8_t data[BUF_SIZE];
        uint32_t rd;
        uint32_t wr;
    };
    typedef struct Uart
    {
        struct ucbuf Rsvbuf;
        struct ucbuf Sndbuf;
        USART_TypeDef *handler;
    } UartDef;

    //extern void vUartHal_Init(void);
    extern void vIRQ_Uart_Handle(UartDef *puart);
    extern int32_t Uart_Read_IT(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t delay);
    extern int32_t Uart_Write_IT(UartDef *puart, const uint8_t *buf, uint32_t count, uint32_t delay);

#endif
