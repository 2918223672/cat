#include "lcd_app.h"
#include "lcdfont.h"
#include "gif_data_new.h"

extern SPI_HandleTypeDef hspi2;

/******************************************************************************
 * 函数说明：在指定区域内填充颜色
 * 入口参数：xsta,ysta  起始坐标
 *           xend,yend  终止坐标
 *           color      填充颜色
 ******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
    u16 i, j;
    LCD_Address_Set(xsta, ysta, xend - 1, yend - 1);
    for(i = ysta; i < yend; i++)
    {
        for(j = xsta; j < xend; j++)
        {
            LCD_WR_DATA(color);
        }
    }
}

/******************************************************************************
 * 函数说明：在指定位置画点
 ******************************************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_WR_DATA(color);
}

/******************************************************************************
 * 函数说明：画线 (Bresenham算法)
 ******************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if(delta_x > 0) incx = 1;
    else if (delta_x == 0) incx = 0;
    else { incx = -1; delta_x = -delta_x; }
    if(delta_y > 0) incy = 1;
    else if (delta_y == 0) incy = 0;
    else { incy = -1; delta_y = -delta_y; }
    if(delta_x > delta_y) distance = delta_x;
    else distance = delta_y;
    for(t = 0; t < distance + 1; t++)
    {
        LCD_DrawPoint(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if(xerr > distance) { xerr -= distance; uRow += incx; }
        if(yerr > distance) { yerr -= distance; uCol += incy; }
    }
}

/******************************************************************************
 * 函数说明：画矩形
 ******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

/******************************************************************************
 * 函数说明：画圆
 ******************************************************************************/
void Draw_Circle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    a = 0;
    b = r;
    while(a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color); //3
        LCD_DrawPoint(x0 + b, y0 - a, color); //0
        LCD_DrawPoint(x0 - a, y0 + b, color); //1
        LCD_DrawPoint(x0 - a, y0 - b, color); //2
        LCD_DrawPoint(x0 + b, y0 + a, color); //4
        LCD_DrawPoint(x0 + a, y0 - b, color); //5
        LCD_DrawPoint(x0 + a, y0 + b, color); //6
        LCD_DrawPoint(x0 - b, y0 + a, color); //7
        a++;
        if((a * a + b * b) > (r * r)) { b--; }
    }
}

/******************************************************************************
 * 函数说明：显示中文字符串 (自动选择字号)
 * 入口参数：x,y   起点坐标
 *           *s    要显示的中文字符串
 *           fc    字体颜色, bc 背景颜色
 *           sizey 字号 可选 12 16 24 32
 *           mode: 0=非叠加模式 1=叠加模式
 ******************************************************************************/
void LCD_ShowChinese(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    while(*s != 0)
    {
        if(sizey == 12)      LCD_ShowChinese12x12(x, y, s, fc, bc, sizey, mode);
        else if(sizey == 16) LCD_ShowChinese16x16(x, y, s, fc, bc, sizey, mode);
        else if(sizey == 24) LCD_ShowChinese24x24(x, y, s, fc, bc, sizey, mode);
        else if(sizey == 32) LCD_ShowChinese32x32(x, y, s, fc, bc, sizey, mode);
        else return;
        s += 2;
        x += sizey;
    }
}

/******************************************************************************
 * 函数说明：显示12x12中文字符 (使用内建字库)
 ******************************************************************************/
void LCD_ShowChinese12x12(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;

    HZnum = sizeof(tfont12) / sizeof(typFNT_GB12);
    for(k = 0; k < HZnum; k++)
    {
        if((tfont12[k].Index[0] == *(s)) && (tfont12[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for(i = 0; i < TypefaceNum; i++)
            {
                for(j = 0; j < 8; j++)
                {
                    if(!mode)
                    {
                        if(tfont12[k].Msk[i] & (0x01 << j)) LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m % sizey == 0) { m = 0; break; }
                    }
                    else
                    {
                        if(tfont12[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数说明：显示16x16中文字符
 ******************************************************************************/
void LCD_ShowChinese16x16(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont16) / sizeof(typFNT_GB16);
    for(k = 0; k < HZnum; k++)
    {
        if ((tfont16[k].Index[0] == *(s)) && (tfont16[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for(i = 0; i < TypefaceNum; i++)
            {
                for(j = 0; j < 8; j++)
                {
                    if(!mode)
                    {
                        if(tfont16[k].Msk[i] & (0x01 << j)) LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m % sizey == 0) { m = 0; break; }
                    }
                    else
                    {
                        if(tfont16[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数说明：显示24x24中文字符
 ******************************************************************************/
void LCD_ShowChinese24x24(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont24) / sizeof(typFNT_GB24);
    for(k = 0; k < HZnum; k++)
    {
        if ((tfont24[k].Index[0] == *(s)) && (tfont24[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for(i = 0; i < TypefaceNum; i++)
            {
                for(j = 0; j < 8; j++)
                {
                    if(!mode)
                    {
                        if(tfont24[k].Msk[i] & (0x01 << j)) LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m % sizey == 0) { m = 0; break; }
                    }
                    else
                    {
                        if(tfont24[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数说明：显示32x32中文字符
 ******************************************************************************/
void LCD_ShowChinese32x32(u16 x, u16 y, u8 *s, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 i, j, m = 0;
    u16 k;
    u16 HZnum;
    u16 TypefaceNum;
    u16 x0 = x;
    TypefaceNum = (sizey / 8 + ((sizey % 8) ? 1 : 0)) * sizey;
    HZnum = sizeof(tfont32) / sizeof(typFNT_GB32);
    for(k = 0; k < HZnum; k++)
    {
        if ((tfont32[k].Index[0] == *(s)) && (tfont32[k].Index[1] == *(s + 1)))
        {
            LCD_Address_Set(x, y, x + sizey - 1, y + sizey - 1);
            for(i = 0; i < TypefaceNum; i++)
            {
                for(j = 0; j < 8; j++)
                {
                    if(!mode)
                    {
                        if(tfont32[k].Msk[i] & (0x01 << j)) LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m % sizey == 0) { m = 0; break; }
                    }
                    else
                    {
                        if(tfont32[k].Msk[i] & (0x01 << j)) LCD_DrawPoint(x, y, fc);
                        x++;
                        if((x - x0) == sizey) { x = x0; y++; break; }
                    }
                }
            }
        }
        continue;
    }
}

/******************************************************************************
 * 函数说明：显示单个ASCII字符 (使用内建字库)
 ******************************************************************************/
void LCD_ShowChar(u16 x, u16 y, u8 num, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 temp, sizex, t, m = 0;
    u16 i, TypefaceNum;
    u16 x0 = x;
    sizex = sizey / 2;
    TypefaceNum = (sizex / 8 + ((sizex % 8) ? 1 : 0)) * sizey;
    num = num - ' ';
    LCD_Address_Set(x, y, x + sizex - 1, y + sizey - 1);
    for(i = 0; i < TypefaceNum; i++)
    {
        if(sizey == 12)      temp = ascii_1206[num][i];
        else if(sizey == 16) temp = ascii_1608[num][i];
        else if(sizey == 24) temp = ascii_2412[num][i];
        else if(sizey == 32) temp = ascii_3216[num][i];
        else return;
        for(t = 0; t < 8; t++)
        {
            if(!mode)
            {
                if(temp & (0x01 << t)) LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
                m++;
                if(m % sizex == 0) { m = 0; break; }
            }
            else
            {
                if(temp & (0x01 << t)) LCD_DrawPoint(x, y, fc);
                x++;
                if((x - x0) == sizex) { x = x0; y++; break; }
            }
        }
    }
}

/******************************************************************************
 * 函数说明：显示字符串 (ASCII)
 ******************************************************************************/
void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    while(*p != '\0')
    {
        LCD_ShowChar(x, y, *p, fc, bc, sizey, mode);
        x += sizey / 2;
        p++;
    }
}

/******************************************************************************
 * 函数说明：求m的n次方
 ******************************************************************************/
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;
    while(n--) result *= m;
    return result;
}

/******************************************************************************
 * 函数说明：显示整数
 ******************************************************************************/
void LCD_ShowIntNum(u16 x, u16 y, u16 num, u8 len, u16 fc, u16 bc, u8 sizey)
{
    u8 t, temp;
    u8 enshow = 0;
    u8 sizex = sizey / 2;
    for(t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;
        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + t * sizex, y, ' ', fc, bc, sizey, 0);
                continue;
            }
            else enshow = 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
 * 函数说明：显示两位小数浮点数
 ******************************************************************************/
void LCD_ShowFloatNum1(u16 x, u16 y, float num, u8 len, u16 fc, u16 bc, u8 sizey)
{
    u8 t, temp, sizex;
    u16 num1;
    sizex = sizey / 2;
    num1 = num * 100;
    for(t = 0; t < len; t++)
    {
        temp = (num1 / mypow(10, len - t - 1)) % 10;
        if(t == (len - 2))
        {
            LCD_ShowChar(x + (len - 2) * sizex, y, '.', fc, bc, sizey, 0);
            t++;
            len += 1;
        }
        LCD_ShowChar(x + t * sizex, y, temp + 48, fc, bc, sizey, 0);
    }
}

/******************************************************************************
 * 函数说明：显示图片 (RGB565格式，逐像素写入)
 ******************************************************************************/
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
    u16 i, j;
    u32 k = 0;
    LCD_Address_Set(x, y, x + length - 1, y + width - 1);
    for(i = 0; i < length; i++)
    {
        for(j = 0; j < width; j++)
        {
            LCD_WR_DATA8(pic[k * 2]);
            LCD_WR_DATA8(pic[k * 2 + 1]);
            k++;
        }
    }
}


/******************************************************************************
 * 函数说明：显示单色位图 (1bit/pixel, MSB高位在前) — 快速版
 *          整帧数据传输期间 CS 保持低电平, DC 也保持高电平(数据模式)
 * 入口参数：x,y   起始坐标
 *           w,h   图像宽高(像素)
 *           bmp   位图数据 (每字节8像素, 逐行MSB在前)
 *           fc,bc 前景色/背景色 (RGB565)
 ******************************************************************************/
void LCD_ShowMonoBitmap(u16 x, u16 y, u16 w, u16 h, const u8 bmp[], u16 fc, u16 bc)
{
    u16 row, col, bit;
    u16 bytes_per_row = w / 8;

    LCD_Address_Set(x, y, x + w - 1, y + h - 1);
    // Address_Set 结束时 CS 为 HIGH, DC 为 HIGH (数据模式)
    // 之后拉低 CS 一次，连续发送全部像素数据，避免逐像素翻转 CS

    LCD_CS_Clr();  // 拉低 CS，开始像素数据突发传输

    for(row = 0; row < h; row++) {
        for(col = 0; col < bytes_per_row; col++) {
            u8 byte = bmp[row * bytes_per_row + col];
            for(bit = 0; bit < 8; bit++) {
                u16 color = (byte & (0x80 >> bit)) ? fc : bc;
                // 写 2 字节 RGB565 (高字节在前)，不翻 CS
                LCD_Writ_Bus_Bulk(color >> 8);
                LCD_Writ_Bus_Bulk(color & 0xFF);
            }
        }
    }

    LCD_CS_Set();  // 拉高 CS，结束传输
}

/******************************************************************************
 * 函数说明：显示 Image2Lcd 256色索引位图 (直接使用 22.c 等软件生成的数组)
 * 格式: [扫描模式2B][宽度2B][高度2B][颜色数2B][调色板 256×3B RGB][像素索引...]
 * 自动裁剪留白区域 → 缩放填充目标区域，不留白边
 * 入口参数：x,y     目标区域左上角
 *           tw,th   目标区域宽高 (如 LCD 160×128)
 *           data    Image2Lcd 生成的原始数组
 ******************************************************************************/
void LCD_ShowIndexedBitmap(u16 x, u16 y, u16 tw, u16 th, const u8 data[])
{
    u16 sw = data[2] | (data[3] << 8);   // 源图宽度
    u16 sh = data[4] | (data[5] << 8);   // 源图高度

    // 预计算 RGB565 调色板 (RGB888 → RGB565)
    u16 pal[256];
    u16 i;
    for (i = 0; i < 256; i++) {
        u8 r = data[8 + i * 3];
        u8 g = data[8 + i * 3 + 1];
        u8 b = data[8 + i * 3 + 2];
        pal[i] = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    }

    const u8 *pixels = data + 8 + 768;   // 跳过头部 + 调色板

    // 自动裁剪: 扫描非背景像素的边界 (背景 = RGB 均 > 240, 即索引色接近纯白)
    u16 crop_left = sw, crop_top = sh, crop_right = 0, crop_bottom = 0;
    u16 row, col;
    for (row = 0; row < sh; row++) {
        for (col = 0; col < sw; col++) {
            u8 idx = pixels[row * sw + col];
            u8 r = data[8 + idx * 3];
            u8 g = data[8 + idx * 3 + 1];
            u8 b = data[8 + idx * 3 + 2];
            if (r < 240 || g < 240 || b < 240) {  // 非纯白
                if (col < crop_left)  crop_left  = col;
                if (col > crop_right) crop_right = col;
                if (row < crop_top)   crop_top   = row;
                if (row > crop_bottom)crop_bottom= row;
            }
        }
    }
    // 加少许边距
    u16 margin = 2;
    if (crop_left   > margin) crop_left   -= margin; else crop_left   = 0;
    if (crop_top    > margin) crop_top    -= margin; else crop_top    = 0;
    if (crop_right  < sw - 1 - margin) crop_right  += margin; else crop_right  = sw - 1;
    if (crop_bottom < sh - 1 - margin) crop_bottom += margin; else crop_bottom = sh - 1;

    u16 cw = crop_right - crop_left + 1;   // 裁剪后宽度
    u16 ch = crop_bottom - crop_top + 1;   // 裁剪后高度

    if (cw == 0 || ch == 0) { cw = sw; ch = sh; crop_left = 0; crop_top = 0; }

    LCD_Address_Set(x, y, x + tw - 1, y + th - 1);
    LCD_CS_Clr();

    // 将裁剪区域缩放填充到目标区域 (最近邻)
    for (row = 0; row < th; row++) {
        u16 sy = crop_top + (u32)row * ch / th;
        for (col = 0; col < tw; col++) {
            u16 sx = crop_left + (u32)col * cw / tw;
            u8 idx = pixels[sy * sw + sx];
            u16 c = pal[idx];
            LCD_Writ_Bus_Bulk(c >> 8);
            LCD_Writ_Bus_Bulk(c & 0xFF);
        }
    }

    LCD_CS_Set();
}
