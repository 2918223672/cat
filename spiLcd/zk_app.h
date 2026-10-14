#ifndef __ZK_APP_H
#define __ZK_APP_H

#include "lcd_init.h"

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

/* 字库缓冲区 */
extern u8 FontBuf[130];

/* 字库IC (GT30L32S4W) 底层操作函数 */
void ZK_command(u8 dat);
u8   get_data_from_ROM(void);
void get_n_bytes_data_from_ROM(u8 AddrHigh, u8 AddrMid, u8 AddrLow, u8 *pBuff, u8 DataLen);

/* 字库IC 显示函数 */
void Display_GB2312(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc);
void Display_GB2312_String(u16 x, u16 y, u8 zk_num, u8 text[], u16 fc, u16 bc);
void Display_Asc(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc);
void Display_Asc_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc);
void Display_Arial_TimesNewRoman(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc);
void Display_Arial_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc);
void Display_TimesNewRoman_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc);

#endif
