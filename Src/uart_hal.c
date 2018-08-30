
#include "uart_hal.h"
/*----------------------------------------------------------------------
                            内部函数声明
----------------------------------------------------------------------*/
static uint32_t uwBuf_EmpLen(const struct ucbuf *buf);
static uint32_t uwBuf_UnReadLen(const struct ucbuf *buf);
static void vBuf_Clear(struct ucbuf *buf);

/*----------------------------------------------------------------------
                            全局变量定义
----------------------------------------------------------------------*/
UartDef UART_GPRS;

/*----------------------------------------------------------------------
                          外部全局变量声明
----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
                            内部函数定义
----------------------------------------------------------------------*/

/***************************************************************
* 函数名称: vUserHal_Init
* 功能描述: 硬件中间层初始化，所调用函数为CobeMx自动生成
* 输入参数: 
* 输出参数: 
* 返 回 值: 
****************************************************************/
void vUartHal_Init(void)
{
    //LL_SYSTICK_EnableIT();
    UART_GPRS.handler = USART1;
    memset(UART_GPRS.Sndbuf.data, 0, BUF_SIZE);
    UART_GPRS.Sndbuf.wr = 0;
    UART_GPRS.Sndbuf.rd = 0;
    memset(UART_GPRS.Rsvbuf.data, 0, BUF_SIZE);
    UART_GPRS.Rsvbuf.wr = 0;
    UART_GPRS.Rsvbuf.rd = 0;
}

/***************************************************************
* 函数名称: Uart_Read
* 功能描述: 读串口
* 输入参数: puart：串口结构体指针  buf:存储位置   count：读取数量  delay：等待时间ms，可以为0
* 输出参数: 无
* 返 回 值: > 0：实际读取的数量   -1：无数据或count==0或错误 
****************************************************************/
int32_t Uart_Read_IT(UartDef *puart, uint8_t *buf, uint32_t count, uint32_t delay)
{
    uint32_t readlen = 0;
    uint16_t i;
    if (!count || count > 0xFFFF)
        return -1;
    LL_USART_EnableIT_RXNE(puart->handler);
    readlen = uwBuf_UnReadLen(&puart->Rsvbuf);
    while (delay)
    {
        readlen = uwBuf_UnReadLen(&puart->Rsvbuf);
        /*多余的字节本次丢弃，并调用BufClear防止后面再读取*/
        if (readlen >= count)
            break;
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            delay--;
        }
    }
    LL_USART_DisableIT_RXNE(puart->handler);
    if (readlen)
    {
        /*实际读到的长度*/
        readlen = (readlen < count ? readlen : count);
        for (i = 0; i < readlen; i++)
        {
            *(buf + i) = puart->Rsvbuf.data[puart->Rsvbuf.rd];
            puart->Rsvbuf.rd = (++puart->Rsvbuf.rd) & BUF_SIZE;
        }
        return readlen;
    }
    else
        return -1;
}

/***************************************************************
* 函数名称: Uart_Send
* 功能描述: 写串口
* 输入参数: uart：系统串口  buf:存储位置   count：发送数量
* 输出参数: 无
* 返 回 值: > 0：实际发送的数量   -1：发送失败
****************************************************************/

int32_t Uart_Write_IT(UartDef *puart, const uint8_t *buf, uint32_t count, uint32_t delay)
{
    uint32_t sendlen = 0;
    uint16_t i;
    //保证count不为0，可至少开启一次发送中断
    if (!count || count > uwBuf_EmpLen(&puart->Sndbuf))
        return -1;
    for (i = 0; i < count; i++)
    {
        puart->Sndbuf.data[puart->Sndbuf.wr] = *(buf + i);
        puart->Sndbuf.wr = (++puart->Sndbuf.wr) & BUF_SIZE;
    }
    sendlen = puart->Sndbuf.rd;
    LL_USART_EnableIT_TXE(puart->handler);
    while (delay)
    {
        /*发送完成*/
        if (!uwBuf_UnReadLen(&puart->Sndbuf))
            break;
        if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
        {
            delay--;
        }
    }
    /*规定时间内未发送完成，也要关闭中断*/
    LL_USART_DisableIT_TXE(puart->handler);
    sendlen = (puart->Sndbuf.rd - sendlen + BUF_SIZE) & BUF_SIZE;
    return sendlen;
}

/***************************************************************
* 函数名称: vIRQ_Uart_Handle
* 功能描述: 串口中断处理
* 输入参数: 自定义串口类型指针
* 输出参数: 
* 返 回 值: 
****************************************************************/
void vIRQ_Uart_Handle(UartDef *puart)
{
    if (LL_USART_IsActiveFlag_ORE(puart->handler))
    {
        LL_USART_ClearFlag_ORE(puart->handler);
    }
    /*接收*/
    if (LL_USART_IsActiveFlag_RXNE(puart->handler) &&
        LL_USART_IsEnabledIT_RXNE(puart->handler))
    {
        /*读取后自动清中断标志位*/
        if (uwBuf_EmpLen(&puart->Rsvbuf) == 0)
            return;
        else
        {
            puart->Rsvbuf.data[puart->Rsvbuf.wr] = LL_USART_ReceiveData8(puart->handler);
            puart->Rsvbuf.wr = ++puart->Rsvbuf.wr & BUF_SIZE;
        }
    }
    /*发送*/
    if (LL_USART_IsActiveFlag_TXE(puart->handler) && //是否可以去掉
        LL_USART_IsEnabledIT_TXE(puart->handler))
    {
        LL_USART_TransmitData8(puart->handler, puart->Sndbuf.data[puart->Sndbuf.rd]);
        puart->Sndbuf.rd = ++puart->Sndbuf.rd & BUF_SIZE;
        if (uwBuf_UnReadLen(&puart->Sndbuf) == 0)
        {
            LL_USART_DisableIT_TXE(puart->handler);
        }
    }
}

/********************************************************************
* 功    能：查询驱动层缓存剩余空间
* 输    入：数据缓存结构体地址
* 输    出：uint32_t
*           
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
uint32_t uwBuf_EmpLen(const struct ucbuf *buf)
{
    return ((buf->rd + BUF_SIZE - buf->wr) % BUF_SIZE - 1);
}
/********************************************************************
* 功    能：查询缓存中未读的字节数
* 输    入：数据缓存结构体地址
* 输    出：uint32_t
*          
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
uint32_t uwBuf_UnReadLen(const struct ucbuf *buf)
{
    return (buf->wr - buf->rd + BUF_SIZE) % BUF_SIZE;
}
/********************************************************************
* 功    能：清空缓存
* 输    入：数据缓存结构体地址
* 输    出：
*          
* 编 写 人：
* 编写日期：2016.6.28
**********************************************************************/
void vBuf_Clear(struct ucbuf *buf)
{
    buf->rd = buf->wr;
}