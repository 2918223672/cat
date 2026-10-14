#ifndef encoder_h
#define encoder_h 

#include "main.h"
#include "tim.h"
#include <stdint.h>

#define PI            3.14159265f
#define SAMPLING_TIME 0.01f       // 采样时间 dt = 10ms (与 floor.c 里的中断周期严格对应)

/* ----- 机械与硬件参数定义 ----- */
#define WHEEL_DIAMETER  6.5f      // 轮子直径(cm)
// 编码器线数(13) * 减速比(28) * 4倍频(定时器双边沿捕获) = 1456 (请根据你的实际电机参数修改！)
#define ENCODER_PPR     1456.0f   

/* 定时器、方向与滤波参数统一在头文件中配置。 */
#define ENCODER_LEFT_TIMER       htim3
#define ENCODER_RIGHT_TIMER      htim4
#define ENCODER_LEFT_DIRECTION   (-1)
#define ENCODER_RIGHT_DIRECTION  (1)
#define ENCODER_SPEED_ALPHA      0.8f
#define ENCODER_DEAD_ZONE        3

/* ----- 外部变量声明 (让 PID 模块能拿到真实速度) ----- */
extern volatile float left_speed_cm_s;
extern volatile float right_speed_cm_s;

/* ----- 函数声明 ----- */
void Encoder_Init(void);
void Encoder_GetSpeed(void);

#endif

