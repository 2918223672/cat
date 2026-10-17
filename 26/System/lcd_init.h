#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "Myconfig.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

/* 屏幕方向设置: 0/2=竖屏  1/3=横屏 */

#define USE_HORIZONTAL 3

#if USE_HORIZONTAL==0 || USE_HORIZONTAL==2
#define LCD_W 128
#define LCD_H 160
#else
#define LCD_W 160
#define LCD_H 128
#endif

//-----------------LCD 端口定义 (SPI2 + CubeMX生成)----------------
// SPI2:  PB13=SCK  PB15=MOSI  PB14=MISO  (硬件SPI全双工)

#define LCD_CS_Clr()    HAL_GPIO_WritePin(Lcd_CS_GPIO_Port, Lcd_CS_Pin, GPIO_PIN_RESET) // CS = PB12
#define LCD_CS_Set()    HAL_GPIO_WritePin(Lcd_CS_GPIO_Port, Lcd_CS_Pin, GPIO_PIN_SET)

#define LCD_DC_Clr()    HAL_GPIO_WritePin(Lcd_D_C_GPIO_Port, Lcd_D_C_Pin, GPIO_PIN_RESET) // DC = PC5
#define LCD_DC_Set()    HAL_GPIO_WritePin(Lcd_D_C_GPIO_Port, Lcd_D_C_Pin, GPIO_PIN_SET)

// LCD无独立RES引脚，由电源上电复位

#define LCD_BLK_Clr()   HAL_GPIO_WritePin(Lcd_BLK_GPIO_Port, Lcd_BLK_Pin, GPIO_PIN_RESET) // BLK = PB1
#define LCD_BLK_Set()   HAL_GPIO_WritePin(Lcd_BLK_GPIO_Port, Lcd_BLK_Pin, GPIO_PIN_SET)

// 字库SPI与LCD共用SPI2，通过CS区分:
#define ZK_CS_Set()     HAL_GPIO_WritePin(ZK_CS_GPIO_Port, ZK_CS_Pin, GPIO_PIN_RESET)  // Set=低电平=选通
#define ZK_CS_Clr()     HAL_GPIO_WritePin(ZK_CS_GPIO_Port, ZK_CS_Pin, GPIO_PIN_SET)    // Clr=高电平=取消

/* 函数声明 */
void LCD_Writ_Bus(u8 dat);                      // SPI字节写入 (单字节带CS控制)
void LCD_Writ_Bus_Bulk(u8 dat);                 // SPI裸写 (不控制CS, 用于连续数据突发)
void LCD_WR_DATA8(u8 dat);                      // 写入一个字节
void LCD_WR_DATA(u16 dat);                      // 写入两个字节
void LCD_WR_REG(u8 dat);                        // 写入一个指令
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2); // 设置坐标函数
void LCD_Init(void);                            // LCD初始化

#endif
