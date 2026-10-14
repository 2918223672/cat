#include "pid.h"

PID_TypeDef left_pid;
PID_TypeDef right_pid;

// 在调度器里调用的初始化
void PID_System_Init(void) {

    // Ki 大幅降低：防止编码器噪声积分累积导致直道来回振荡
    PID_Init(&left_pid, PID_LEFT_DEFAULT_KP, PID_LEFT_DEFAULT_KI, PID_LEFT_DEFAULT_KD);
    PID_Init(&right_pid, PID_RIGHT_DEFAULT_KP, PID_RIGHT_DEFAULT_KI, PID_RIGHT_DEFAULT_KD);
}

// PID 初始化
void PID_Init(PID_TypeDef *pid, float kp, float ki, float kd) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    pid->target = 0.0f;

    pid->error[0] = 0.0f;
    pid->error[1] = 0.0f;
    pid->error[2] = 0.0f;

    pid->output = 0.0f;
    pid->delta = 0.0f;   // 增量值 (仅增量式 PID 使用)

    pid->out_min = -PWM_PERIOD;
    pid->out_max = PWM_PERIOD;

}

// 设置目标值
void PID_SetTarget(PID_TypeDef *pid, float target) {
    pid->target = target;
}

// 计算增量
float PID_Compute_Increment(PID_TypeDef *pid, float current_speed) {

    pid->error[0] = pid->target - current_speed;

    // 积分分离：误差绝对值较大时不积分，避免启动阶段积分饱和导致过冲振荡
    // 用线性衰减替代硬开关，避免阈值处积分跳变
    float integral_term = 0.0f;
    float abs_err = (pid->error[0] >= 0) ? pid->error[0] : -pid->error[0];
    if (abs_err < PID_INTEGRAL_SEPARATION) {
        float sep_factor = 1.0f - (abs_err / PID_INTEGRAL_SEPARATION);
        integral_term = pid->ki * pid->error[0] * sep_factor;
    }

    pid->delta = pid->kp * (pid->error[0] - pid->error[1])
               + integral_term
               + pid->kd * (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);

    pid->output += pid->delta;

    if (pid->output > pid->out_max) pid->output = pid->out_max;
    if (pid->output < pid->out_min) pid->output = pid->out_min;

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];

    return pid->output;
}
