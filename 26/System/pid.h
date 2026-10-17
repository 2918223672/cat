#ifndef Pid_h
#define Pid_h

#include "Myconfig.h"

#define PWM_PERIOD 100.0f //motor.c 里有 *10

typedef struct
{
    float kp;
    float ki;
    float kd;

    float target;

    float error[3];

    float output;       // 最终输出占空比
    float delta;        // 增量值
    
    float out_max;      // ARR最大值+100
    float out_min;      // ARR最小值-100

} PID_TypeDef;

extern PID_TypeDef left_pid;
extern PID_TypeDef right_pid;

void PID_System_Init(void);
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd);
void PID_SetTarget(PID_TypeDef *pid, float target);
float PID_Compute_Increment(PID_TypeDef *pid, float current_speed);


#endif

