#include "software_iic.h"

/* 简单微秒延时（168MHz 下 ~10us） */
static void IIC_Delay(void)
{
    uint32_t i;
    for (i = 0; i < 160; i++) {
        __NOP();
    }
}

/* 软件I2C基础函数 */
void IIC_Start(void)
{
    SDA_HIGH();
    SCL_HIGH();
    IIC_Delay();
    SDA_LOW();
    IIC_Delay();
    SCL_LOW();
}

void IIC_Stop(void)
{
    SDA_LOW();
    IIC_Delay();
    SCL_HIGH();
    IIC_Delay();
    SDA_HIGH();
    IIC_Delay();
}

unsigned char IIC_WaitAck(void)
{
    unsigned char ack;
    SDA_HIGH();
    SCL_HIGH();
    IIC_Delay();
    ack = READ_SDA();
    SCL_LOW();
    IIC_Delay();
    return ack;
}

void IIC_SendAck(void)
{
    SDA_LOW();
    SCL_HIGH();
    IIC_Delay();
    SCL_LOW();
    SDA_HIGH();
}

void IIC_SendNAck(void)
{
    SDA_HIGH();
    SCL_HIGH();
    IIC_Delay();
    SCL_LOW();
}

unsigned char IIC_SendByte(unsigned char dat)
{
    for(unsigned char i = 0; i < 8; i++) {
        (dat & 0x80) ? SDA_HIGH() : SDA_LOW();
        dat <<= 1;
        SCL_HIGH();
        IIC_Delay();
        SCL_LOW();
        IIC_Delay();
    }
    return IIC_WaitAck();
}

unsigned char IIC_RecvByte(void)
{
    unsigned char dat = 0;
    SDA_HIGH();
    /* 接收数据前切换SDA为输入模式 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(SDA_PORT, &GPIO_InitStruct);
    for(unsigned char i = 0; i < 8; i++) {
        dat <<= 1;
        SCL_HIGH();
        IIC_Delay();
        if(READ_SDA()) dat |= 0x01;
        SCL_LOW();
        IIC_Delay();
    }

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    HAL_GPIO_Init(SDA_PORT, &GPIO_InitStruct);
    return dat;
}

/* 应用层函数 */
unsigned char IIC_ReadByte(unsigned char Salve_Address)
{
    unsigned char dat;

    IIC_Start();
    IIC_SendByte(Salve_Address | 0x01);  // 读模式
    dat = IIC_RecvByte();
    IIC_SendNAck();
    IIC_Stop();

    return dat;
}

unsigned char IIC_ReadBytes(unsigned char Salve_Address, unsigned char Reg_Address,
                          unsigned char *Result, unsigned char len)
{
    IIC_Start();
    if(IIC_SendByte(Salve_Address & 0xFE)) {  // 写模式
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(Reg_Address)) {
        IIC_Stop();
        return 0;
    }
    IIC_Start();
    if(IIC_SendByte(Salve_Address | 0x01)) {  // 读模式
        IIC_Stop();
        return 0;
    }

    for(unsigned char i = 0; i < len; i++) {
        Result[i] = IIC_RecvByte();
        (i == len-1) ? IIC_SendNAck() : IIC_SendAck();
    }
    IIC_Stop();
    return 1;
}

unsigned char IIC_WriteByte(unsigned char Salve_Address, unsigned char Reg_Address,
                          unsigned char data)
{
    IIC_Start();
    if(IIC_SendByte(Salve_Address & 0xFE)) {  // 写模式
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(Reg_Address)) {
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(data)) {
        IIC_Stop();
        return 0;
    }
    IIC_Stop();
    return 1;
}

unsigned char IIC_WriteBytes(unsigned char Salve_Address, unsigned char Reg_Address,
                           unsigned char *data, unsigned char len)
{
    IIC_Start();
    if(IIC_SendByte(Salve_Address & 0xFE)) {
        IIC_Stop();
        return 0;
    }
    if(IIC_SendByte(Reg_Address)) {
        IIC_Stop();
        return 0;
    }

    for(unsigned char i = 0; i < len; i++) {
        if(IIC_SendByte(data[i])) {
            IIC_Stop();
            return 0;
        }
    }
    IIC_Stop();
    return 1;
}

/* ====== 感为智能传感器应用层 ====== */

unsigned char Ping(void)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF<<1, GW_GRAY_PING, &dat, 1);
    if(dat == GW_GRAY_PING_OK)
    {
        return 0;
    }
    else return 1;
}

unsigned char IIC_Get_Digtal(void)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF<<1, GW_GRAY_DIGITAL_MODE, &dat, 1);
    return dat;
}

unsigned char IIC_Get_Anolog(unsigned char * Result, unsigned char len)
{
    if(IIC_ReadBytes(GW_GRAY_ADDR_DEF<<1, GW_GRAY_ANALOG_BASE_, Result, len)) return 1;
    else return 0;
}

unsigned char IIC_Get_Single_Anolog(unsigned char Channel)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF<<1, GW_GRAY_ANALOG(Channel), &dat, 1);
    return dat;
}

unsigned char IIC_Anolog_Normalize(uint8_t Normalize_channel)
{
    return IIC_WriteBytes(GW_GRAY_ADDR_DEF<<1, 0xCF, &Normalize_channel, 1);
}

unsigned short IIC_Get_Offset(void)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF<<1, Offset, &dat, 1);
    return dat;
}
