# STM32F407VET6 双轮智能小车模块

本仓库整理了基于 **STM32F407VET6、STM32 HAL 和 CubeMX** 的双轮智能小车核心代码，包括电机与编码器、速度 PID、JY901S IMU、匿名上位机通信、ST7735 LCD、外置字库以及裸机周期调度器。

仓库中的 5 个模块可以按需独立移植；`月薪猫tft` 目录保留原有综合工程，便于对照完整应用。

## 功能概览

- 双直流电机 PWM、H 桥方向控制和正交编码器测速
- 左右轮增量式速度 PID，支持积分分离和输出限幅
- JY901S 加速度、角速度、姿态角、磁场和温度解析
- 匿名上位机 V7 风格数据发送、参数读取和 PID 在线调节
- ST7735 LCD 绘图、文字、数值、RGB565 和索引图片显示
- GT30L32S4W 外置字库及内建中英文字模
- 基于 `HAL_GetTick()` 的裸机多周期任务调度

## 目录结构

| 目录 | 主要内容 |
|---|---|
| `ano` | 匿名上位机通信、数据帧收发和 PID 参数协议 |
| `jy901s` | JY901S UART DMA 空闲接收与传感器数据解析 |
| `motor` | 双电机、编码器测速和左右轮速度 PID |
| `Schedular_app` | 按键、循迹控制和 LCD 刷新的裸机周期调度 |
| `spiLcd` | ST7735 LCD、GT30L32S4W 字库、字模和图片资源 |
| `月薪猫tft` | 原有综合工程与应用代码 |

各目录均附带独立 `README.md`，详细说明接口、配置宏和接入方法。

## 默认硬件资源

| 功能 | 默认外设与引脚 |
|---|---|
| 电机 PWM | TIM2_CH1 / PA0，TIM2_CH2 / PA1 |
| 电机方向 | PC4、PB0、PD10、PD11 |
| 左轮编码器 | TIM3_CH1/CH2，PA6、PA7 |
| 右轮编码器 | TIM4_CH1/CH2，PD12、PD13 |
| JY901S | UART5_TX / PC12，UART5_RX / PD2，RX DMA |
| 匿名上位机 | USART3_TX / PB10，USART3_RX / PB11 |
| LCD 与字库总线 | SPI2：PB13 / PB14 / PB15 |
| LCD 控制 | CS / PB12，DC / PC5，BLK / PB1 |
| 外置字库片选 | PD8 |

> 上表对应当前代码的默认配置。移植时应以 CubeMX 工程和各模块头文件中的资源宏为准。若串口、定时器或引脚被其他外设占用，请先在 CubeMX 中重新分配，再修改对应宏。

## 模块依赖

- `motor` 依赖 HAL GPIO、TIM PWM 和编码器模式。
- `ano` 依赖 UART；启用 PID 参数读写时还依赖 `motor/pid.c` 和 `motor/pid.h`。
- `jy901s` 依赖 UART RX DMA、UART 中断和 `HAL_UARTEx_RxEventCallback()`。
- `spiLcd` 依赖 SPI2 与 GPIO；LCD 和外置字库共用总线，通过独立片选区分。
- `Schedular_app` 是当前小车的应用调度层，依赖电机、编码器、PID、LCD、按键、循迹传感器和应用任务函数。

## 快速接入

### 1. 配置 CubeMX

根据需要启用以下资源：

- TIM2：两路 PWM 输出
- TIM3、TIM4：Encoder Mode
- USART3：匿名上位机通信
- UART5：JY901S，开启 RX DMA、UART 中断和 ReceiveToIdle
- SPI2：ST7735 LCD 与 GT30L32S4W 字库
- 电机方向、LCD 控制和字库片选 GPIO

### 2. 添加代码

将需要的目录加入工程，并把对应 `.c` 文件加入编译。确保 CubeMX 生成的 `main.h`、`gpio.h`、`tim.h`、`usart.h`、`spi.h` 等头文件可被模块访问。

### 3. 初始化

如果不使用应用调度器，可以分别初始化：

```c
Encoder_Init();
motor_init();
PID_System_Init();
LCD_Init();
JY901S_Init();
```

如果使用 `Schedular_app`，在 CubeMX 外设初始化完成后调用：

```c
JY901S_Init();       /* 使用 IMU 时调用 */
Scheduler_Init();    /* 内部初始化编码器、电机、PID、LCD 和应用任务 */

while (1)
{
    Scheduler_Run();
    vJY901S_Data_Proc();
}
```

> 同一个模块的初始化函数不要重复调用。`Scheduler_Init()` 已包含当前应用所需的电机、编码器、PID 和 LCD 初始化。

## UART 回调接入

JY901S 使用 DMA 空闲接收：

```c
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    JY901S_RxEventCallback(huart, size);
}
```

匿名上位机可使用单字节接收中断：

```c
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &ANO_UART_HANDLE) {
        Ano_GetByte(ano_rx_byte);
        HAL_UART_Receive_IT(&ANO_UART_HANDLE, &ano_rx_byte, 1);
    }
}
```

如果工程中有多个 UART 模块，应在同一个 HAL 回调中根据 `huart` 分发，避免重复定义回调函数。

## 10 ms 电机控制示例

```c
Encoder_GetSpeed();

PID_SetTarget(&left_pid, 30.0f);
PID_SetTarget(&right_pid, 30.0f);

float left_out = PID_Compute_Increment(&left_pid, left_speed_cm_s);
float right_out = PID_Compute_Increment(&right_pid, right_speed_cm_s);

Left_Dirt((int16_t)left_out);
Right_Dirt((int16_t)right_out);
```

## 移植注意事项

- 优先修改各模块头文件中的硬件映射宏，不要在多个 `.c` 文件中分散修改资源。
- `ENCODER_PPR` 必须按编码器线数、减速比和倍频后的实测脉冲数配置。
- `gif_data_new.h` 和 `lcdfont.h` 是资源文件，体积较大，不需要对应的 `.c` 文件。
- 外置字库字符串需要使用 GB2312 编码字节。
- LCD 与字库共享 SPI2，访问前必须正确切换片选；多任务使用时需要保证不会并发访问。
- 裸机调度任务应保持短小，避免阻塞主循环。
- 首次上电调试时先架空车轮并限制 PWM，确认电机方向、编码器符号和 PID 参数后再落地运行。

## 文档

更详细的接口、宏定义和示例请查看各模块目录中的说明文档：

- [`ano/README.md`](./ano/README.md)
- [`jy901s/README.md`](./jy901s/README.md)
- [`motor/README.md`](./motor/README.md)
- [`Schedular_app/README.md`](./Schedular_app/README.md)
- [`spiLcd/README.md`](./spiLcd/README.md)
