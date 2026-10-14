# SPI LCD、外置字库与图片资源

本目录包含 ST7735 LCD 底层驱动、LCD 绘图层、GT30L32S4W 外置字库驱动，以及内建字模和月薪猫图片资源。

## 文件组成

| 文件 | 功能 |
|---|---|
| `lcd_init.c/.h` | ST7735 初始化、命令/数据写入、地址窗口和 SPI 底层操作 |
| `lcd_app.c/.h` | 点线矩形圆、文字、数字、RGB565、单色位图和256色索引图显示 |
| `zk_app.c/.h` | GT30L32S4W 字库读取，支持 GB2312、ASCII、Arial、Times New Roman |
| `lcdfont.h` | 12/16/24/32 点阵中英文字模数据 |
| `gif_data_new.h` | 月薪猫和界面图片数组，例如 `gImage_22` |

`lcdfont.h` 和 `gif_data_new.h` 是只读资源文件，不需要对应 `.c`。

## 当前引脚定义

LCD 与字库共用 SPI2，通过独立片选区分：

| 功能 | STM32 引脚 | CubeMX 名称 |
|---|---|---|
| SPI2_SCK | PB13 | 硬件 SPI 时钟 |
| SPI2_MISO | PB14 | 字库数据输出；LCD 通常不用 |
| SPI2_MOSI | PB15 | LCD/字库数据输入 |
| LCD_CS | PB12 | `Lcd_CS_Pin` |
| LCD_DC | PC5 | `Lcd_D_C_Pin` |
| LCD_BLK | PB1 | `Lcd_BLK_Pin` |
| 字库_CS | PD8 | `ZK_CS_Pin` |

LCD 没有单独的复位引脚，初始化时使用 ST7735 软件复位命令。

所有硬件映射集中在 `lcd_init.h`：

```c
#define LCD_SPI_HANDLE       hspi2
#define LCD_SPI_TIMEOUT_MS   10U
#define USE_HORIZONTAL       3

#define LCD_CS_Clr()   HAL_GPIO_WritePin(Lcd_CS_GPIO_Port, Lcd_CS_Pin, GPIO_PIN_RESET)
#define LCD_CS_Set()   HAL_GPIO_WritePin(Lcd_CS_GPIO_Port, Lcd_CS_Pin, GPIO_PIN_SET)
#define LCD_DC_Clr()   HAL_GPIO_WritePin(Lcd_D_C_GPIO_Port, Lcd_D_C_Pin, GPIO_PIN_RESET)
#define LCD_DC_Set()   HAL_GPIO_WritePin(Lcd_D_C_GPIO_Port, Lcd_D_C_Pin, GPIO_PIN_SET)
```

注意原项目为保持兼容，`ZK_CS_Set()` 表示拉低并选中字库，`ZK_CS_Clr()` 表示拉高并释放字库。

## SPI2 配置

原工程参数：主机模式、双线全双工、8 位数据、CPOL Low、CPHA 1Edge、软件 NSS、MSB First、预分频 8。

`lcd_init.h` 已直接包含 `main.h`、`spi.h` 和 `stdint.h`，不依赖原工程的 `Myconfig.h`。

## 初始化与基本使用

CubeMX 初始化 GPIO 和 SPI2 后调用：

```c
LCD_Init();
LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
```

显示文字和数值：

```c
LCD_ShowString(4, 4, (const u8 *)"Salary Cat", WHITE, BLACK, 16, 0);
LCD_ShowIntNum(4, 24, 1234, 4, YELLOW, BLACK, 16);
LCD_ShowFloatNum1(4, 44, 12.3f, 4, GREEN, BLACK, 16);
```

显示 RGB565 图片：

```c
LCD_ShowPicture(0, 0, image_width, image_height, image_data);
```

显示外置字库字符串：

```c
uint8_t text[] = "月薪猫"; /* 必须使用 GB2312 编码字节 */
Display_GB2312_String(0, 0, 2, text, WHITE, BLACK);
```

## 屏幕方向和尺寸

`USE_HORIZONTAL` 可设为 0～3：

- 0、2：竖屏，逻辑尺寸 128×160。
- 1、3：横屏，逻辑尺寸 160×128。

修改方向后，应用层绘图应始终使用 `LCD_W`、`LCD_H`，不要写死屏幕宽高。

## 注意事项

- 读取字库前必须释放 LCD 片选；代码已通过 `LCD_CS_Set()` 和字库 CS 宏处理共享总线。
- `gif_data_new.h` 体积较大，只在需要月薪猫图片时加入编译。
- `LCD_ShowIndexedBitmap()` 读取的是带调色板的256色索引数据，不等同于普通 GIF 文件解码。
- 多个任务同时使用 SPI2 时，需要在应用层增加互斥或保证调用不会并发。
