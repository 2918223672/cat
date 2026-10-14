# ANO 匿名上位机通信模块

本目录提供匿名上位机 V7 风格的数据发送、参数读写和逐字节收包代码。

## 文件

- `ano.c`：协议组帧、校验、UART 发送、接收状态机和 PID 参数解析。
- `ano.h`：公开接口以及 UART、协议 ID、缓冲区等宏定义。

## 当前硬件定义

原工程使用 USART3，CubeMX 引脚为：

| 信号 | STM32 引脚 | 代码名称 |
|---|---|---|
| USART3_TX | PB10 | `ANO_UART_HANDLE` / `huart3` |
| USART3_RX | PB11 | `ANO_UART_INSTANCE` / `USART3` |

串口参数为 115200、8 数据位、1 停止位、无校验、无流控。移植时集中修改 `ano.h`：

```c
#define ANO_UART_HANDLE          huart3
#define ANO_UART_INSTANCE        USART3
#define ANO_UART_TIMEOUT_MS      100U
#define ANO_BUFFER_SIZE          32U
```

`ANO_UART_INSTANCE` 用于 STM32F4 的 `SR/DR` 寄存器直接发送。如果移植到寄存器结构不同的 MCU，应把 `Ano_SentPar()` 和 `Ano_SentCheck()` 中的直接寄存器发送改成 `HAL_UART_Transmit()`。

## 主要接口

```c
void Data_send(int32_t a, int32_t b, int32_t c, int32_t d);
void Ano_SentPar(uint16_t id, int32_t data);
void Ano_SentCheck(uint8_t id, uint8_t sc, uint8_t ac);
void Ano_GetByte(uint8_t data);
void Ano_Anl(uint8_t *packet);
```

- `Data_send()`：发送 4 个 32 位通道数据，用于上位机波形显示。
- `Ano_GetByte()`：每收到一个字节调用一次，内部自动拼接完整数据帧。
- `Ano_Anl()`：校验完整帧并处理参数读取、写入。
- 参数 ID 1～4 对应左轮 PID，5～8 对应右轮 PID。

## 依赖

`ano.h` 已直接包含 HAL 串口声明和 PID 头文件：

```c
#include "ano.h"
```

由于参数协议直接访问 `left_pid`、`right_pid`，使用参数读写功能时需要同时加入 `motor` 目录中的 `pid.c/.h`。

## 使用方法

初始化 USART3 后即可发送：

```c
Data_send((int32_t)left_speed_cm_s,
          (int32_t)right_speed_cm_s,
          (int32_t)left_pid.target,
          (int32_t)right_pid.target);
```

接收时可以采用单字节中断，并在回调中持续重新开启接收：

```c
static uint8_t ano_rx_byte;

HAL_UART_Receive_IT(&ANO_UART_HANDLE, &ano_rx_byte, 1);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &ANO_UART_HANDLE) {
        Ano_GetByte(ano_rx_byte);
        HAL_UART_Receive_IT(&ANO_UART_HANDLE, &ano_rx_byte, 1);
    }
}
```

## 注意事项

- 协议按小端字节序拆分 32 位数据。
- 接收状态机限制有效负载不超过 `ANO_BUFFER_SIZE - 6`，防止接收数组越界。
- `Ano_Anl()` 中的参数映射属于当前双轮 PID 项目；其他项目可保留协议层并重写参数 ID 的 `switch`。
- 不要在两个源文件中重复实现同一个 HAL UART 回调，应把不同串口的处理合并到一个回调中。
