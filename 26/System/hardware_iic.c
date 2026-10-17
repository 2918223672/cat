#include "hardware_iic.h"

unsigned char IIC_ReadByte(unsigned char Slave_Address)
{
    unsigned char dat;
    HAL_I2C_Master_Receive(&hi2c1, Slave_Address, &dat, 1, 1000);
    return dat;
}

unsigned char IIC_ReadBytes(unsigned char Slave_Address, unsigned char Reg_Address,
                            unsigned char *Result, unsigned char len)
{
    return HAL_I2C_Mem_Read(&hi2c1, Slave_Address, Reg_Address,
                            I2C_MEMADD_SIZE_8BIT, Result, len, 5) == HAL_OK;
}

unsigned char IIC_WriteByte(unsigned char Slave_Address, unsigned char Reg_Address,
                            unsigned char data)
{
    unsigned char dat[2] = {Reg_Address, data};
    return HAL_I2C_Master_Transmit(&hi2c1, Slave_Address, dat, 2, 1000) == HAL_OK;
}

unsigned char IIC_WriteBytes(unsigned char Slave_Address, unsigned char Reg_Address,
                             unsigned char *data, unsigned char len)
{
    return HAL_I2C_Mem_Write(&hi2c1, Slave_Address, Reg_Address,
                             I2C_MEMADD_SIZE_8BIT, data, len, 1000) == HAL_OK;
}

unsigned char Ping(void)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF << 1, GW_GRAY_PING, &dat, 1);
    if (dat == GW_GRAY_PING_OK)
        return 0;
    else
        return 1;
}

unsigned char IIC_Get_Digtal(void)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF << 1, GW_GRAY_DIGITAL_MODE, &dat, 1);
    return dat;
}

unsigned char IIC_Get_Anolog(unsigned char *Result, unsigned char len)
{
    if (IIC_ReadBytes(GW_GRAY_ADDR_DEF << 1, GW_GRAY_ANALOG_BASE_, Result, len))
        return 1;
    else
        return 0;
}

unsigned char IIC_Get_Single_Anolog(unsigned char Channel)
{
    unsigned char dat;
    IIC_ReadBytes(GW_GRAY_ADDR_DEF << 1, GW_GRAY_ANALOG(Channel), &dat, 1);
    return dat;
}

unsigned char IIC_Anolog_Normalize(uint8_t Normalize_channel)
{
    return IIC_WriteBytes(GW_GRAY_ADDR_DEF << 1, 0xCF, &Normalize_channel, 1);
}

unsigned short IIC_Get_Offset(void)
{
    unsigned char dat[2] = {0};
    IIC_ReadBytes(GW_GRAY_ADDR_DEF << 1, Offset, dat, 2);
    return (unsigned short)dat[0] | (unsigned short)dat[1] << 8;
}
