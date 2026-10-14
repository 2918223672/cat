#include "zk_app.h"
#include "lcd_init.h"  // 借用里面的 LCD_WR_DATA, LCD_Address_Set 等底层刷屏函数

u8 FontBuf[130]; // 字库数据缓存区

/******************************************************************************
 * 函数说明：向字库IC写入命令/数据 (硬件SPI)
 ******************************************************************************/
void ZK_command(u8 dat)
{
    // 发送1字节数据，超时时间10ms
    HAL_SPI_Transmit(&LCD_SPI_HANDLE, &dat, 1, LCD_SPI_TIMEOUT_MS);
}

/******************************************************************************
 * 函数说明：从字库IC读取数据 (硬件SPI) —— 【核心排错点】
 ******************************************************************************/
u8 get_data_from_ROM(void)
{
    u8 rx_data = 0;
    u8 tx_dummy = 0xFF; // 必须发送伪字节(0xFF)去产生时钟信号
    
    // 同时发送 0xFF 并接收返回的字库数据
    HAL_SPI_TransmitReceive(&LCD_SPI_HANDLE, &tx_dummy, &rx_data, 1, LCD_SPI_TIMEOUT_MS);
    
    return rx_data;
}

/******************************************************************************
 * 函数说明：读取N个字节的数据
 * 入口数据：AddrHigh/Mid/Low  24位起始地址
 * *pBuff            存放数据的缓存区指针
 * DataLen           读取长度
 ******************************************************************************/
void get_n_bytes_data_from_ROM(u8 AddrHigh, u8 AddrMid, u8 AddrLow, u8 *pBuff, u8 DataLen)
{
    u8 i;
    
    // 【关键】：必须先拉高LCD的CS，释放SPI总线，防止屏幕把字库数据吃掉！
    LCD_CS_Set();
    
    // 拉低字库芯片的CS，选中字库
    ZK_CS_Set();
    
    ZK_command(0x03);           // 写入标准 SPI Flash 读指令(0x03)
    ZK_command(AddrHigh);       // 写入地址高字节
    ZK_command(AddrMid);        // 写入地址中字节
    ZK_command(AddrLow);        // 写入地址低字节
    
    for(i = 0; i < DataLen; i++)
    {
        *(pBuff + i) = get_data_from_ROM(); // 连续读取
    }
    
    // 拉高字库芯片的CS，取消选中
    ZK_CS_Clr();
}

/******************************************************************************
 * 函数说明：显示单个GB2312汉字
 ******************************************************************************/
void Display_GB2312(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc)
{
    u8 i, k;
    switch(zk_num)
    {
    case 1: // 12*12
        LCD_Address_Set(x, y, x + 11, y + 11);
        for(i = 0; i < 24; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;  
    case 2: // 15*16
        LCD_Address_Set(x, y, x + 15, y + 15);
        for(i = 0; i < 32; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;     
    case 3: // 24*24
        LCD_Address_Set(x, y, x + 23, y + 23);
        for(i = 0; i < 72; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;     
    case 4: // 32*32
        LCD_Address_Set(x, y, x + 31, y + 31);
        for(i = 0; i < 128; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;    
    }
}

/******************************************************************************
 * 函数说明：显示GB2312编码的汉字字符串
 ******************************************************************************/
void Display_GB2312_String(u16 x, u16 y, u8 zk_num, u8 text[], u16 fc, u16 bc)
{
    u8 i = 0;
    u8 AddrHigh, AddrMid, AddrLow ; 
    u32 FontAddr = 0; 
    u32 BaseAdd = 0; 
    u8 n, d; 
    
    switch(zk_num)
    {
    case 1 : BaseAdd = 0x00;    n = 24;  d = 12; break; // 12*12
    case 2 : BaseAdd = 0x2C9D0; n = 32;  d = 16; break; // 15*16
    case 3 : BaseAdd = 0x68190; n = 72;  d = 24; break; // 24*24
    case 4 : BaseAdd = 0xEDF00; n = 128; d = 32; break; // 32*32
    }
    
    while((text[i] > 0x00))
    {
        if(((text[i] >= 0xA1) && (text[i] <= 0xA9)) && (text[i + 1] >= 0xA1))
        {
            FontAddr = (text[i] - 0xA1) * 94;
            FontAddr += (text[i + 1] - 0xA1);
            FontAddr = (unsigned long)((FontAddr * n) + BaseAdd);
            
            AddrHigh = (FontAddr & 0xff0000) >> 16; 
            AddrMid = (FontAddr & 0xff00) >> 8;  
            AddrLow = FontAddr & 0xff;       
            get_n_bytes_data_from_ROM(AddrHigh, AddrMid, AddrLow, FontBuf, n); 
            Display_GB2312(x, y, zk_num, fc, bc); 
        }
        else if(((text[i] >= 0xB0) && (text[i] <= 0xF7)) && (text[i + 1] >= 0xA1))
        {
            FontAddr = (text[i] - 0xB0) * 94;
            FontAddr += (text[i + 1] - 0xA1) + 846;
            FontAddr = (unsigned long)((FontAddr * n) + BaseAdd);
            
            AddrHigh = (FontAddr & 0xff0000) >> 16; 
            AddrMid = (FontAddr & 0xff00) >> 8;  
            AddrLow = FontAddr & 0xff;       
            get_n_bytes_data_from_ROM(AddrHigh, AddrMid, AddrLow, FontBuf, n); 
            Display_GB2312(x, y, zk_num, fc, bc); 
        }
        x += d; 
        i += 2; 
    }
}

/******************************************************************************
 * 函数说明：显示单个标准 ASCII 字符
 ******************************************************************************/
void Display_Asc(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc)
{
    unsigned char i, k;
    switch(zk_num)
    {
    case 1: // 5x7 ASCII
        LCD_Address_Set(x, y, x + 7, y + 7);
        for(i = 0; i < 7; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;
    case 2: // 7x8 ASCII
        LCD_Address_Set(x, y, x + 7, y + 7);
        for(i = 0; i < 8; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;   
    case 3: // 6x12 ASCII
        LCD_Address_Set(x, y, x + 7, y + 11);
        for(i = 0; i < 12; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;  
    case 4: // 8x16 ASCII
        LCD_Address_Set(x, y, x + 7, y + 15);
        for(i = 0; i < 16; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;     
    case 5: // 12x24 ASCII
        LCD_Address_Set(x, y, x + 15, y + 23);
        for(i = 0; i < 48; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;    
    case 6: // 16x32 ASCII
        LCD_Address_Set(x, y, x + 15, y + 31);
        for(i = 0; i < 64; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;   
    }
}

/******************************************************************************
 * 函数说明：显示标准 ASCII 字符串
 ******************************************************************************/
void Display_Asc_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc)
{
    u8 i = 0;
    u8 AddrHigh, AddrMid, AddrLow ; 
    u32 FontAddr = 0; 
    u32 BaseAdd = 0; 
    u8 n, d; 
    switch(zk_num)
    {
    case 1: BaseAdd = 0x1DDF80; n = 8;  d = 6;  break;  // 5x7 ASCII
    case 2: BaseAdd = 0x1DE280; n = 8;  d = 8;  break;  // 7x8 ASCII
    case 3: BaseAdd = 0x1DBE00; n = 12; d = 6;  break;  // 6x12 ASCII
    case 4: BaseAdd = 0x1DD780; n = 16; d = 8;  break;  // 8x16 ASCII
    case 5: BaseAdd = 0x1DFF00; n = 48; d = 12; break;  // 12x24 ASCII
    case 6: BaseAdd = 0x1E5A50; n = 64; d = 16; break;  // 16x32 ASCII
    }
    
    while((text[i] > 0x00))
    {
        if((text[i] >= 0x20) && (text[i] <= 0x7E))
        {
            FontAddr =  text[i] - 0x20;
            FontAddr = (unsigned long)((FontAddr * n) + BaseAdd);
            
            AddrHigh = (FontAddr & 0xff0000) >> 16; 
            AddrMid = (FontAddr & 0xff00) >> 8;  
            AddrLow = FontAddr & 0xff;       
            get_n_bytes_data_from_ROM(AddrHigh, AddrMid, AddrLow, FontBuf, n ); 
            Display_Asc(x, y, zk_num, fc, bc); 
        }
        i++;  
        x += d; 
    }
}

/******************************************************************************
 * 函数说明：显示 Arial / Times New Roman ASCII 字符
 ******************************************************************************/
void Display_Arial_TimesNewRoman(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc)
{
    unsigned char i, k;
    switch(zk_num)
    {
    case 1: // 6x12
        LCD_Address_Set(x, y, x + 15, y + 11);
        for(i = 2; i < 26; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;  
    case 2: // 8x16
        LCD_Address_Set(x, y, x + 15, y + 15);
        for(i = 2; i < 34; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;     
    case 3: // 12x24
        LCD_Address_Set(x, y, x + 23, y + 23);
        for(i = 2; i < 74; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;    
    case 4: // 16x32
        LCD_Address_Set(x, y, x + 31, y + 31);
        for(i = 2; i < 130; i++) {
            for(k = 0; k < 8; k++) {
                if((FontBuf[i] & (0x80 >> k)) != 0) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
            }
        }
        break;   
    }
}

/******************************************************************************
 * 函数说明：显示 Arial ASCII 字符串
 ******************************************************************************/
void Display_Arial_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc)
{
    u8 i = 0;
    u8 AddrHigh, AddrMid, AddrLow ; 
    u32 FontAddr = 0; 
    u32 BaseAdd = 0; 
    u8 n, d; 
    switch(zk_num)
    {
    case 1: BaseAdd = 0x1DC400; n = 26;  d = 8;  break;  // 8x12 Arial
    case 2: BaseAdd = 0x1DE580; n = 34;  d = 12; break;  // 12x16 Arial
    case 3: BaseAdd = 0x1E22D0; n = 74;  d = 16; break;  // 16x24 Arial
    case 4: BaseAdd = 0x1E99D0; n = 130; d = 24; break;  // 24x32 Arial
    }
    while((text[i] > 0x00))
    {
        if((text[i] >= 0x20) && (text[i] <= 0x7E))
        {
            FontAddr =  text[i] - 0x20;
            FontAddr = (unsigned long)((FontAddr * n) + BaseAdd);
            
            AddrHigh = (FontAddr & 0xff0000) >> 16; 
            AddrMid = (FontAddr & 0xff00) >> 8;  
            AddrLow = FontAddr & 0xff;       
            get_n_bytes_data_from_ROM(AddrHigh, AddrMid, AddrLow, FontBuf, n ); 
            Display_Arial_TimesNewRoman(x, y, zk_num, fc, bc); 
        }
        i++;  
        x += d; 
    }
}

/******************************************************************************
 * 函数说明：显示 Times New Roman ASCII 字符串
 ******************************************************************************/
void Display_TimesNewRoman_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc)
{
    u8 i = 0;
    u8 AddrHigh, AddrMid, AddrLow ; 
    u32 FontAddr = 0; 
    u32 BaseAdd = 0; 
    u8 n, d; 
    switch(zk_num)
    {
    case 1: BaseAdd = 0x1DCDC0; n = 26;  d = 8;  break;  // 8x12 TNR
    case 2: BaseAdd = 0x1DF240; n = 34;  d = 12; break;  // 12x16 TNR
    case 3: BaseAdd = 0x1E3E90; n = 74;  d = 16; break;  // 16x24 TNR
    case 4: BaseAdd = 0x1ECA90; n = 130; d = 24; break;  // 24x32 TNR
    }
    while((text[i] > 0x00))
    {
        if((text[i] >= 0x20) && (text[i] <= 0x7E))
        {
            FontAddr =  text[i] - 0x20;
            FontAddr = (unsigned long)((FontAddr * n) + BaseAdd);
            
            AddrHigh = (FontAddr & 0xff0000) >> 16; 
            AddrMid = (FontAddr & 0xff00) >> 8;  
            AddrLow = FontAddr & 0xff;       
            get_n_bytes_data_from_ROM(AddrHigh, AddrMid, AddrLow, FontBuf, n ); 
            Display_Arial_TimesNewRoman(x, y, zk_num, fc, bc); 
        }
        i++;  
        x += d; 
    }
}

