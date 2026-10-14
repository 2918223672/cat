#ifndef __LCD_APP_H
#define __LCD_APP_H

#include "lcd_init.h"

/* ========== 类型定义 (兼容SPL风格) ========== */
#ifndef u8
typedef uint8_t  u8;
#endif
#ifndef u16
typedef uint16_t u16;
#endif
#ifndef u32
typedef uint32_t u32;
#endif

/* ========== 标准色 ========== */
#define WHITE          0xFFFF  // 白 (R=31, G=63, B=31)
#define BLACK          0x0000  // 黑 (R=0,  G=0,  B=0)
#define RED            0xF800  // 红 (R=31, G=0,  B=0)
#define GREEN          0x07E0  // 绿 (R=0,  G=63, B=0)
#define BLUE           0x001F  // 蓝 (R=0,  G=0,  B=31)
#define YELLOW         0xFFE0  // 黄 (R=31, G=63, B=0)
#define MAGENTA        0xF81F  // 洋红/紫 (R=31, G=0, B=31)
#define CYAN           0x07FF  // 青 (R=0,  G=63, B=31)

/* ========== 衍生色 ========== */
#define GRAY           0x8410  // 灰 (R=16, G=32, B=16)
#define DARKBLUE       0x01CF  // 深蓝
#define LIGHTBLUE      0x7D7C  // 浅蓝
#define DARKGREEN      0x03E0  // 深绿
#define LIGHTGREEN     0x87F0  // 浅绿
#define BROWN          0xBC40  // 棕
#define ORANGE         0xFD20  // 橙

/* ========== 基本绘图函数 (lcd_app.c) ========== */
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color); // 填充指定矩形区域
void LCD_DrawPoint(u16 x, u16 y, u16 color);                     // 在指定坐标画一个像素点
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);        // 画一条直线
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color);  // 画一个空心矩形框
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color);               // 以(x0,y0)为圆心画空心圆

/* ========== 内建字库显示函数 (lcd_app.c) ========== */
/* (x,y)显示坐标，(u8 *)"LCD_W: " 显示字符
   fc 字体颜色，bc 背景颜色，sizey 字体大小（12/16/24/32）
  mode 显示模式（0 代表非叠加模式，带有红色背景底框；1 代表透明背景的叠加模式）。
  */
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode);          // 显示单个内建汉字(自适应)
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示12x12点阵内建汉字
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示16x16点阵内建汉字
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示24x24点阵内建汉字
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示32x32点阵内建汉字

void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode);          // 显示单个标准ASCII字符
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode);   // 显示内建ASCII字符串
u32 mypow(u8 m, u8 n);                                                               // 乘方计算辅助函数(m^n)
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey);        // 显示指定位数的整数
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey);   // 显示指定位数的浮点数(保留1位小数)

void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[]);          // 显示RGB565格式图片
void LCD_ShowMonoBitmap(u16 x, u16 y, u16 w, u16 h, const u8 bmp[], u16 fc, u16 bc); // 显示单色位图(1bit/px, MSB)
void LCD_ShowIndexedBitmap(u16 x, u16 y, u16 tw, u16 th, const u8 data[]); // 显示Image2Lcd 256色索引位图(全彩)

/* ========== 外部字库IC函数 (zk_app.c) ========== */
/* (x,y)显示坐标，(u8)zk_num 显示字符
   fc 字体颜色，bc 背景颜色
zk_num 字体大小
   在中文函数 (GB2312) 中：1 代表 12x12大小，2 代表 15x16大小，3 代表 24x24大小，4 代表 32x32大小。
   在英文函数 (Asc) 中：通常 1=5x7，2=7x8，3=6x12，4=8x16，5=12x24，6=16x32。
   在特殊字体 (Arial / TimesNewRoman) 中：通常 1=8x12，2=12x16，3=16x24，4=24x32。
*/
void ZK_command(u8 dat);                                                                // 向硬件字库IC发送单字节命令/地址
u8 get_data_from_ROM(void);                                                             // 从硬件字库IC读取单字节数据
void get_n_bytes_data_from_ROM(u8 AddrHigh, u8 AddrMid, u8 AddrLow, u8 *pBuff, u8 DataLen); // 从字库指定物理地址读取N个字节
void Display_GB2312(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc);                           // 从外部字库检索并显示单个汉字
void Display_GB2312_String(u16 x, u16 y, u8 zk_num, u8 text[], u16 fc, u16 bc);         // 检索并显示一串GB2312编码的汉字字符串
void Display_Asc(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc);                              // 从外部字库检索并显示单个ASCII字符
void Display_Asc_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc);           // 检索并显示一串标准ASCII字符串
void Display_Arial_TimesNewRoman(u16 x, u16 y, u8 zk_num, u16 fc, u16 bc);              // 显示Arial或TimesNewRoman单个西文字符
void Display_Arial_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc);         // 以Arial西文字体显示一串字符串
void Display_TimesNewRoman_String(u16 x, u16 y, u16 zk_num, u8 text[], u16 fc, u16 bc); // 以TimesNewRoman字体显示一串字符串

#endif
