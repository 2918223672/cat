# JY901S IMU 驱动

本目录用于接收和解析 JY901S 连续输出的加速度、角速度、姿态角、磁场和温度数据。

## 文件

- `jy901s.c`：UART DMA 空闲接收、44 字节数据处理、校验和数据解析。
- `jy901s.h`：寄存器地址、数据结构、串口配置宏和公开接口。

## 当前硬件定义

原工程使用 UART5：

| 信号 | STM32 引脚 | 说明 |
|---|---|---|
| UART5_TX | PC12 | MCU 发送到 JY901S RX，仅读取时可不接 |
| UART5_RX | PD2 | 接收 JY901S TX |

串口参数为 115200、8N1，接收方式为 DMA + ReceiveToIdle。对应宏位于 `jy901s.h`：

```c
#define JY901S_UART_HANDLE          huart5
#define JY901S_RX_LENGTH            44U
#define JY901S_DISABLE_DMA_HT_IRQ   1U
```

如果 UART5 已被步进电机或其他设备占用，必须在 CubeMX 中选择另一个带 RX DMA 的串口，并修改 `JY901S_UART_HANDLE`。

## 主要接口

```c
void JY901S_Init(void);
void JY901S_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size);
void vJY901S_Data_Proc(void);
void JY901S_Task(void);
```

- `JY901S_Init()`：启动 44 字节 DMA 空闲接收。
- `JY901S_RxEventCallback()`：接收事件处理，标记完整帧并重新开启 DMA。
- `vJY901S_Data_Proc()`：校验并更新各个全局数据结构。
- `JY901S_Task()`：处理数据并通过 `printf` 输出 Roll、Pitch、Yaw。

解析后的数据位于：

```c
extern SAcc    jy901_Acc;
extern SGyro   jy901_Gyro;
extern SAngle  jy901_Angle;
extern SMag    jy901_Mag;
```

## 接入方法

在 CubeMX 外设初始化完成后调用：

```c
JY901S_Init();
```

将统一的 HAL UART 空闲接收回调转发给驱动：

```c
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    JY901S_RxEventCallback(huart, size);

    /* 其他串口的处理可以继续写在这里。 */
}
```

在主循环或周期任务中解析：

```c
vJY901S_Data_Proc();

int16_t roll  = jy901_Angle.Angle[0];
int16_t pitch = jy901_Angle.Angle[1];
int16_t yaw   = jy901_Angle.Angle[2];
```

## 依赖与配置

- `jy901s.h` 直接包含 `main.h`、`usart.h` 和 `stdint.h`，不依赖原工程的 `Myconfig.h`。
- UART RX DMA 必须在 CubeMX 中启用，UART 中断也必须开启。
- JY901S 应配置为连续输出加速度、角速度、角度和磁场四种 11 字节帧，合计 44 字节。
- 当前结构体中的加速度、角速度、角度和磁场数组元素为 `short`，因此计算后的结果会按整数保存；若需要小数精度，可把对应数组改为 `float`。

## 循迹辅助函数

原工程在 `jy901s.c` 末尾混有一个依赖 `track_algo` 的循迹误差函数。为了让 IMU 驱动可以单独引用，默认关闭：

```c
#define JY901S_ENABLE_TRACK_HELPER 0U
```

确实需要该函数时，将宏改为 `1U`，并确保工程中已经提供 `Track_State_t`、`Track_Ctrl_t` 和 `track_steering()`。
