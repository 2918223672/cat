# 双直流电机、编码器与速度 PID

本目录提供双轮小车的电机输出、正交编码器测速和左右轮增量式速度 PID。

## 文件组成

| 文件 | 功能 |
|---|---|
| `motor.c/.h` | TIM2 双路 PWM、H 桥方向控制、速度限幅和停止 |
| `encoder.c/.h` | TIM3/TIM4 正交编码器、轮速换算、死区和低通滤波 |
| `pid.c/.h` | 左右轮增量式 PID、积分分离和输出限幅 |

## 当前引脚与定时器

### PWM 和方向

| 功能 | STM32 引脚/资源 | CubeMX 名称 |
|---|---|---|
| 左电机 PWM | PA0 / TIM2_CH1 | `MOTOR_LEFT_PWM_CHANNEL` |
| 右电机 PWM | PA1 / TIM2_CH2 | `MOTOR_RIGHT_PWM_CHANNEL` |
| 左电机 IN1 | PC4 | `motor_AO1_Pin` |
| 左电机 IN2 | PB0 | `motor_AO2_Pin` |
| 右电机 IN1 | PD10 | `motor_BO1_Pin` |
| 右电机 IN2 | PD11 | `motor_BO2_Pin` |

### 编码器

| 编码器 | STM32 引脚 | 定时器 |
|---|---|---|
| 左轮 A/B | PA6、PA7 | TIM3 CH1/CH2 |
| 右轮 A/B | PD12、PD13 | TIM4 CH1/CH2 |

## 头文件配置宏

电机资源在 `motor.h` 中配置：

```c
#define MOTOR_PWM_TIMER          htim2
#define MOTOR_LEFT_PWM_CHANNEL   TIM_CHANNEL_1
#define MOTOR_RIGHT_PWM_CHANNEL  TIM_CHANNEL_2
#define MOTOR_SPEED_MAX          100
#define MOTOR_PWM_SCALE          10U
```

方向 GPIO 也通过 `MOTOR_LEFT_IN1_PORT` 等宏统一映射，移植时不需要修改 `motor.c`。

编码器资源和机械参数在 `encoder.h` 中配置：

```c
#define WHEEL_DIAMETER           6.5f
#define ENCODER_PPR              1456.0f
#define ENCODER_LEFT_TIMER       htim3
#define ENCODER_RIGHT_TIMER      htim4
#define ENCODER_LEFT_DIRECTION   (-1)
#define ENCODER_RIGHT_DIRECTION  (1)
#define ENCODER_SPEED_ALPHA      0.8f
#define ENCODER_DEAD_ZONE        3
```

如果某侧速度方向相反，只修改对应的 `ENCODER_*_DIRECTION` 为 `1` 或 `-1`。

PID 默认参数位于 `pid.h`：

```c
#define PID_LEFT_DEFAULT_KP       0.60f
#define PID_LEFT_DEFAULT_KI       0.10f
#define PID_LEFT_DEFAULT_KD       0.00f
#define PID_RIGHT_DEFAULT_KP      0.60f
#define PID_RIGHT_DEFAULT_KI      0.10f
#define PID_RIGHT_DEFAULT_KD      0.00f
#define PID_INTEGRAL_SEPARATION   60.0f
```

## CubeMX 配置要求

- TIM2 配置为 PWM Generation CH1、CH2。原工程 Prescaler 为 3，Period 为 1049。
- TIM3、TIM4 配置为 Encoder Mode，计数周期 65535。
- 四个方向控制引脚配置为推挽输出。
- `MOTOR_PWM_SCALE=10` 对应输入速度 -100～100 映射到比较值 0～1000。

## 初始化

在 CubeMX 完成 GPIO 和 TIM 初始化后调用：

```c
Encoder_Init();
motor_init();
PID_System_Init();
```

`motor_init()` 启动两路 PWM，并立即调用左右电机速度 0，确保上电不误转。

## 10 ms 控制循环示例

```c
Encoder_GetSpeed();

PID_SetTarget(&left_pid, 30.0f);
PID_SetTarget(&right_pid, 30.0f);

float left_out = PID_Compute_Increment(&left_pid, left_speed_cm_s);
float right_out = PID_Compute_Increment(&right_pid, right_speed_cm_s);

Left_Dirt((int16_t)left_out);
Right_Dirt((int16_t)right_out);
```

停止车辆：

```c
PID_SetTarget(&left_pid, 0.0f);
PID_SetTarget(&right_pid, 0.0f);
Left_Dirt(0);
Right_Dirt(0);
```

## 调参和移植注意事项

- `ENCODER_PPR` 必须填写轮子转一圈时定时器实际累计的脉冲数，需计入编码器线数、减速比和倍频。
- `Encoder_GetSpeed()` 使用实际 `HAL_GetTick()` 时间差计算速度，建议每 10 ms 调用一次。
- `ENCODER_SPEED_ALPHA` 越大响应越快，越小越平滑。
- 电机正反转定义取决于实际接线；若方向相反，可交换 H 桥方向引脚逻辑或电机两根线。
- PID 输出范围为 -100～100，与 `Left_Dirt()`、`Right_Dirt()` 的输入范围一致。
