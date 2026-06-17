#include "lcd_init.h"

extern SPI_HandleTypeDef hspi2;

/******************************************************************************
 * 函数说明：LCD底层SPI字节写入 (硬件SPI2)
 ******************************************************************************/
void LCD_Writ_Bus(u8 dat)
{
    LCD_CS_Clr(); // 拉低片选，开始传输

    // 调用 HAL 库硬件 SPI2 发送 1 字节数据，超时时间设为 10ms
    HAL_SPI_Transmit(&hspi2, &dat, 1, 10);

    LCD_CS_Set(); // 拉高片选，结束传输
}

/******************************************************************************
 * 函数说明：LCD写入一个字节数据
 ******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
    LCD_Writ_Bus(dat);
}

/******************************************************************************
 * 函数说明：LCD写入两个字节数据 (高字节在前)
 ******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
}

/******************************************************************************
 * 函数说明：LCD写入指令
 ******************************************************************************/
void LCD_WR_REG(u8 dat)
{
    LCD_DC_Clr();  // 写指令
    LCD_Writ_Bus(dat);
    LCD_DC_Set();  // 写数据
}

/******************************************************************************
 * 函数说明：设置显示区域 (针对 1.8寸 ST7735 分辨率：128x160)
 * 入口参数：x1,x2  列的起始和结束地址 (0 ~ 127)
 * y1,y2  行的起始和结束地址 (0 ~ 159)
 ******************************************************************************/
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WR_REG(0x2a); // 列地址设置
    LCD_WR_DATA(x1);
    LCD_WR_DATA(x2);
    LCD_WR_REG(0x2b); // 行地址设置
    LCD_WR_DATA(y1);
    LCD_WR_DATA(y2);
    LCD_WR_REG(0x2c); // 存储器写
}

/******************************************************************************
 * 函数说明：1.8寸 LCD初始化 (ST7735驱动IC 专用配置)
 * 引脚初始化由 MX_GPIO_Init() + MX_SPI2_Init() 完成
 ******************************************************************************/
void LCD_Init(void)
{
    // 软件复位 (无硬件RES引脚时必须)
    HAL_Delay(10);               // 上电等待稳定
    LCD_WR_REG(0x01);            // SWRESET 软件复位
    HAL_Delay(150);              // 等待复位完成

    LCD_BLK_Set();   // 打开背光
    HAL_Delay(100);

    //************* ST7735 1.8寸屏 核心初始化序列 **********//
    LCD_WR_REG(0x11); // Sleep out
    HAL_Delay(120);

    LCD_WR_REG(0xB1);
    LCD_WR_DATA8(0x01); LCD_WR_DATA8(0x2C); LCD_WR_DATA8(0x2D);
    LCD_WR_REG(0xB2);
    LCD_WR_DATA8(0x01); LCD_WR_DATA8(0x2C); LCD_WR_DATA8(0x2D);
    LCD_WR_REG(0xB3);
    LCD_WR_DATA8(0x01); LCD_WR_DATA8(0x2C); LCD_WR_DATA8(0x2D);
    LCD_WR_DATA8(0x01); LCD_WR_DATA8(0x2C); LCD_WR_DATA8(0x2D);

    LCD_WR_REG(0xB4); // Column inversion
    LCD_WR_DATA8(0x07);

    LCD_WR_REG(0xC0);
    LCD_WR_DATA8(0xA2); LCD_WR_DATA8(0x02); LCD_WR_DATA8(0x84);
    LCD_WR_REG(0xC1); LCD_WR_DATA8(0xC5);
    LCD_WR_REG(0xC2);
    LCD_WR_DATA8(0x0A); LCD_WR_DATA8(0x00);
    LCD_WR_REG(0xC3);
    LCD_WR_DATA8(0x8A); LCD_WR_DATA8(0x2A);
    LCD_WR_REG(0xC4);
    LCD_WR_DATA8(0x8A); LCD_WR_DATA8(0xEE);

    LCD_WR_REG(0xC5); // VCOM
    LCD_WR_DATA8(0x0E);

    LCD_WR_REG(0x3A); // 16-bit/pixel (RGB565格式)
    LCD_WR_DATA8(0x05);

    LCD_WR_REG(0x36); // 屏幕方向控制
    if(USE_HORIZONTAL==0)      LCD_WR_DATA8(0x08); // 0: 竖屏 (正向)
    else if(USE_HORIZONTAL==1) LCD_WR_DATA8(0x68); // 1: 横屏 (正向)
    else if(USE_HORIZONTAL==2) LCD_WR_DATA8(0xC8); // 2: 竖屏 (反向/倒排)
    else                       LCD_WR_DATA8(0xA8); // 3: 横屏 (反向/倒排)

    LCD_WR_REG(0xE0);
    LCD_WR_DATA8(0x02); LCD_WR_DATA8(0x1c); LCD_WR_DATA8(0x07); LCD_WR_DATA8(0x12);
    LCD_WR_DATA8(0x37); LCD_WR_DATA8(0x32); LCD_WR_DATA8(0x29); LCD_WR_DATA8(0x2d);
    LCD_WR_DATA8(0x29); LCD_WR_DATA8(0x25); LCD_WR_DATA8(0x2B); LCD_WR_DATA8(0x39);
    LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x01); LCD_WR_DATA8(0x03); LCD_WR_DATA8(0x10);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA8(0x03); LCD_WR_DATA8(0x1d); LCD_WR_DATA8(0x07); LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x2E); LCD_WR_DATA8(0x2C); LCD_WR_DATA8(0x29); LCD_WR_DATA8(0x2D);
    LCD_WR_DATA8(0x2E); LCD_WR_DATA8(0x2E); LCD_WR_DATA8(0x37); LCD_WR_DATA8(0x3F);
    LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x00); LCD_WR_DATA8(0x02); LCD_WR_DATA8(0x10);

    LCD_WR_REG(0x29); // Display on
}
