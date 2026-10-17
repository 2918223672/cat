/**
 * @file    cloud.c
 * @brief   车载平衡滚球 — Alpha-Beta 滤波 + 双环级联 PID
 *          外环: 球位置 → PID → 目标角度
 *          内环: 目标角度 × ANGLE_KP → 电机 RPM (无角度传感器，内环退化为纯增益)
 *          周期: 调度器 10ms 触发
 *
 *          移植自 STM32F103 步进工程，适配 STM32F407 主车：
 *          - 相机数据来自 USART2 (cam_rx_buff / cam_frame_flag / cam_rx_count)
 *          - ZDT 电机通讯走 UART5 (X_V2.c 已适配)
 *          - 调试打印通过 USART3 (huart3，printf 串口)
 *          - 电机指令在 zdt_test_proc() 里发送，而非主循环
 */

#include "cloud.h"
#include <stdlib.h>
#include <string.h>

/* ========== 全局变量 ========== */
float ball_current_pos       = 0.0f;
float ball_pos_filt          = 0.0f;
float ball_vel_filt          = 0.0f;
AlphaBeta_t ball_ab;
float pendulum_target_angle  = 0.0f;
float motor_angle_deg        = 0.0f;

float    dbg_dx_raw     = 0.0f;
float    dbg_dx_filt    = 0.0f;
float    dbg_dx_max     = 0.0f;
uint16_t dbg_call_cnt   = 0;
uint16_t dbg_dx_small   = 0;
uint16_t dbg_dx_medium  = 0;
uint16_t dbg_dx_large   = 0;
uint16_t dbg_pos_jump   = 0;
uint16_t dbg_dir_flip   = 0;
uint16_t dbg_deadzone   = 0;
uint16_t dbg_saturate   = 0;
uint16_t dbg_angle_saturate = 0;

char dbg_printf_buf[256];
volatile bool dbg_print_flag = false;
volatile bool motor_cmd_pending  = false;
volatile bool motor_stop_pending = false;
volatile bool ball_pos_updated   = false;
uint8_t  motor_cmd_dir = 0;
float    motor_cmd_vel = 0.0f;

/* 小球目标位置（mm），外部通过 g_ball_target_pos = xxx 设置 */
float g_ball_target_pos = 0.0f;

/* ================================================================
 * Alpha-Beta 滤波器
 *   预测:  pos += vel * dt
 *   残差:  r = meas - pos
 *   更新:  pos += ALPHA * r
 *          vel += BETA  * r / dt
 *
 *   大 α → 信任测量值 → 响应快/噪声大 (上限 1.0 = 不过滤)
 *   大 β → 速度跟踪快 → 同时也放大噪声
 * ================================================================ */
void ab_init(AlphaBeta_t *ab)
{
    ab->pos    = 0.0f;
    ab->vel    = 0.0f;
    ab->inited = 0;
}

void ab_update(AlphaBeta_t *ab, float meas)
{
    float r;

    if (!ab->inited) {
        ab->pos    = meas;
        ab->vel    = 0.0f;
        ab->inited = 1;
        return;
    }

    /* 预测: 匀速模型 */
    ab->pos += ab->vel * AB_DT;

    /* 更新: 限幅单帧跳变 >15mm 视为噪声 */
    r = meas - ab->pos;
    if (r >  15.0f) r =  15.0f;
    if (r < -15.0f) r = -15.0f;
    ab->pos += AB_ALPHA * r;
    ab->vel += AB_BETA  * r / AB_DT;
}

/* ================================================================
 * 解析 USART2 相机帧，更新 ball_current_pos
 *   格式: "samples:f1,f2,f3,f4\n" 或 "-1.547\n"
 *   单位 cm → mm (x10), 钳位 ±12.5cm
 * ================================================================ */
void cloud_proc_cam_data(void)
{
    char str[CAM_BUFF_LEN + 1];
    uint8_t len;
    int i;
    char *p_num;
    float val_cm;

    len = cam_rx_count;
    if (len >= CAM_BUFF_LEN) len = CAM_BUFF_LEN;

    memcpy(str, (char *)cam_rx_buff, len);
    str[len] = '\0';

    /* 去尾部 CR/LF */
    for (i = len - 1; i >= 0; i--) {
        if (str[i] == '\r' || str[i] == '\n')
            str[i] = '\0';
        else
            break;
    }

    /* 若有 "samples:" 前缀，跳过 */
    p_num = str;
    {
        char *colon = strchr(str, ':');
        if (colon != NULL) p_num = colon + 1;
    }

    val_cm = (float)atof(p_num);
    if (val_cm >  12.5f) val_cm =  12.5f;
    if (val_cm < -12.5f) val_cm = -12.5f;

    ball_current_pos = val_cm * 10.0f;  /* cm → mm */
    ball_pos_updated = true;            /* 通知控制环有新视觉帧 */
}

/* ================================================================
 * 双环级联 PID 控制 — 10ms 周期
 *
 *   STEP 0: Alpha-Beta 滤波
 *   STEP 1: 调试统计
 *   STEP 2: 外环 → 球位置误差 → PID → target_angle (°)
 *   STEP 3+4: target_angle × ANGLE_KP → 电机 RPM
 *   STEP 5: 设标志 → zdt_test_proc 发 X_V2_Vel_Control
 *   STEP 6: 调试打印计数
 * ================================================================ */
void ball_proc(float ball_target_pos)
{
    /* ======== 外环状态 ======== */
    static float pos_error_last  = 0.0f;
    static float pos_integral    = 0.0f;
    static float pos_prev        = 0.0f;
    static uint8_t pos_inited    = 0;
    static uint8_t in_hold       = 0;   /* 死区迟滞状态 */

    float pos_error;
    float target_angle;
    float motor_vel, abs_vel;

    if (!pos_inited) {
        pos_inited = 1;
        ab_init(&ball_ab);
        pos_prev = ball_current_pos;
        return;
    }

    dbg_call_cnt++;

    /* ==== STEP 0: Alpha-Beta 滤波 ====
     * 只有收到新视觉帧时才做测量更新，否则只做预测
     * 视觉 50fps = 20ms/帧，控制环 100Hz = 10ms；每两帧控制才有一次新数据 */
    if (ball_pos_updated) {
        ball_pos_updated = false;
        ab_update(&ball_ab, ball_current_pos);   /* 预测 + 测量更新 */
    } else {
        ball_ab.pos += ball_ab.vel * AB_DT;      /* 纯预测 */
    }
    ball_pos_filt = ball_ab.pos;
    ball_vel_filt = ball_ab.vel;

    /* ==== STEP 1: 调试统计 ==== */
    dbg_dx_raw  = ball_vel_filt;
    dbg_dx_filt = ball_vel_filt;
    {
        float a = (ball_vel_filt >= 0.0f) ? ball_vel_filt : -ball_vel_filt;
        if (a > dbg_dx_max) dbg_dx_max = a;
        if (a >= 100.0f)     dbg_dx_large++;
        else if (a >= 50.0f) dbg_dx_medium++;
        else if (a >= 10.0f) dbg_dx_small++;
    }
    {
        float j = ball_pos_filt - pos_prev;
        if (j < 0.0f) j = -j;
        if (j > 5.0f) dbg_pos_jump++;
    }
    pos_prev = ball_pos_filt;

    /* ==== STEP 2: 外环 — 球位置误差 → 目标角度 (°) ====
     * 无 KD 项：e=xd-x, ė=-v → -Kd*ė=+Kd*v 会削弱速度阻尼并产生微分尖峰
     * 死区：位置 AND 速度均小于阈值才进入保持态；带迟滞防止振荡 */
    pos_error = ball_target_pos - ball_pos_filt;
    {
        float abs_pos   = (pos_error     >= 0.0f) ? pos_error     : -pos_error;
        float abs_vel_b = (ball_vel_filt >= 0.0f) ? ball_vel_filt : -ball_vel_filt;

        if (!in_hold) {
            if (abs_pos < POS_DEADZONE_MM && abs_vel_b < POS_HOLD_VEL_MM) {
                in_hold = 1;
                dbg_deadzone++;
            }
        } else {
            if (abs_pos > POS_DEADZONE_EXIT_MM || abs_vel_b > POS_HOLD_VEL_MM * 1.5f) {
                in_hold = 0;
            }
        }
    }

    if (in_hold) {
        target_angle = 0.0f;
        pendulum_target_angle = 0.0f;
        pos_error_last = pos_error;
    } else {
        pos_integral += pos_error * 0.01f;
        if (pos_integral >  POS_MAX_INTEGRAL) pos_integral =  POS_MAX_INTEGRAL;
        if (pos_integral < -POS_MAX_INTEGRAL) pos_integral = -POS_MAX_INTEGRAL;

        target_angle = POS_KP * pos_error
                     + POS_KI * pos_integral
                     - POS_KV * ball_vel_filt;

        if (target_angle >  PENDULUM_MAX_DEG) {
            target_angle =  PENDULUM_MAX_DEG;
            dbg_saturate++;
        }
        if (target_angle < -PENDULUM_MAX_DEG) {
            target_angle = -PENDULUM_MAX_DEG;
            dbg_saturate++;
        }
        pos_error_last = pos_error;
        pendulum_target_angle = target_angle;
    }

    /* ==== STEP 3+4: 内环 — target_angle × ANGLE_KP → 电机 RPM ====
     * 无角度传感器，内环退化为纯增益 */
    motor_vel = ANGLE_KP * target_angle;

    if (motor_vel >  ANGLE_MAX_OUT) { motor_vel =  ANGLE_MAX_OUT; dbg_angle_saturate++; }
    if (motor_vel < -ANGLE_MAX_OUT) { motor_vel = -ANGLE_MAX_OUT; dbg_angle_saturate++; }

    /* ==== STEP 5: 电机速度指令标志 ====
     * 不在此处直接发 DMA，而是设标志，由 zdt_test_proc() 在调度器层发送
     * 这样与步进工程"主循环发送"等价，均不在 ISR 内触发 DMA TX */
    abs_vel = (motor_vel >= 0.0f) ? motor_vel : -motor_vel;
    motor_angle_deg += motor_vel * 0.06f;

    if (abs_vel < 0.1f) {
        if (motor_cmd_pending || motor_cmd_vel > 0.0f) {
            motor_stop_pending = true;
        }
        motor_cmd_pending = false;
        motor_cmd_vel     = 0.0f;
    } else {
        if (abs_vel > MOTOR_MAX_SPEED) abs_vel = MOTOR_MAX_SPEED;
        motor_cmd_dir      = (motor_vel >= 0.0f) ? 0 : 1;
        motor_cmd_vel      = abs_vel;
        motor_cmd_pending  = true;
        motor_stop_pending = false;
    }

    /* ==== STEP 6: 调试打印计数 ==== */
    if (dbg_call_cnt % DBG_PRINT_INTERVAL == 0)
        dbg_print_flag = true;
}

/* ================================================================
 * dx_to_pulse — 球位置误差 → 步进脉冲频率 完整链路
 *
 *   dx(mm) → 外环PID → target_angle(°) → 内环P → rpm → pulse_freq(Hz)
 *
 * 调用示例:
 *   static float s_integral = 0.0f, s_err_last = 0.0f;
 *   MotorCmd_t cmd;
 *   dx_to_pulse(ball_target_pos - ball_pos_filt, ball_vel_filt,
 *               0.0f, &s_integral, &s_err_last, &cmd);
 *   X_V2_Vel_Control(MOTOR_ADDR, cmd.dir, MOTOR_ACCEL, cmd.abs_rpm, 0);
 * ================================================================ */
void dx_to_pulse(float dx,
                 float ball_vel,
                 float actual_angle,
                 float *pos_integral,
                 float *pos_err_last,
                 MotorCmd_t *cmd)
{
    float dx_derivative, target_angle, angle_error, rpm;

    /* 外环: dx → 目标倾斜角度 */
    *pos_integral += dx * 0.01f;
    if (*pos_integral >  POS_MAX_INTEGRAL) *pos_integral =  POS_MAX_INTEGRAL;
    if (*pos_integral < -POS_MAX_INTEGRAL) *pos_integral = -POS_MAX_INTEGRAL;

    dx_derivative  = (dx - *pos_err_last) / 0.01f;
    *pos_err_last  = dx;

    target_angle   = POS_KP * dx
                   + POS_KI * (*pos_integral)
                   - POS_KV * ball_vel;

    if (target_angle >  PENDULUM_MAX_DEG) target_angle =  PENDULUM_MAX_DEG;
    if (target_angle < -PENDULUM_MAX_DEG) target_angle = -PENDULUM_MAX_DEG;

    cmd->target_angle = target_angle;

    /* 内环: 角度误差 → 电机 RPM */
    angle_error = target_angle - actual_angle;
    rpm         = ANGLE_KP * angle_error;

    if (rpm >  ANGLE_MAX_OUT) rpm =  ANGLE_MAX_OUT;
    if (rpm < -ANGLE_MAX_OUT) rpm = -ANGLE_MAX_OUT;

    cmd->rpm     = rpm;
    cmd->dir     = (rpm >= 0.0f) ? 0 : 1;
    cmd->abs_rpm = (rpm >= 0.0f) ? rpm : -rpm;

    /* RPM → 步进脉冲频率: freq = RPM × PULSES_PER_REV / 60 */
    cmd->pulse_freq = rpm * (float)MOTOR_PULSES_PER_REV / 60.0f;
}

/* ================================================================
 * zdt_test_proc — 调度器任务入口（10ms 周期）
 *
 *   1. 若有新相机帧，先解析球位置
 *   2. 运行 PID 控制
 *   3. 在调度器层（主循环上下文）发送电机指令
 *      — 步进工程在 main while(1) 里发；此处等价，同样不在 ISR 内
 * ================================================================ */
void zdt_test_proc(float ruler)
{
    /* ---- 斜坡跟踪：目标位置平滑逼近 ruler ----
     * 每次调用最多移动 RAMP_STEP_MM，防止目标跳变对小球产生冲击
     * 速度限制 = RAMP_STEP_MM / 0.01s = 50mm/s
     * 如需更快响应可调大 RAMP_STEP_MM；更平滑可调小 */
    #define RAMP_STEP_MM  0.5f
    static float current_target = 0.0f;
    {
        float step = ruler - current_target;
        if      (step >  RAMP_STEP_MM) step =  RAMP_STEP_MM;
        else if (step < -RAMP_STEP_MM) step = -RAMP_STEP_MM;
        current_target += step;
    }

    /* 若 USART2 IDLE 中断置位了 cam_frame_flag，先解析相机数据 */
    if (cam_frame_flag) {
        cam_frame_flag = false;
        cloud_proc_cam_data();
    }

    /* 运行平衡 PID 控制，目标位置使用平滑后的 current_target */
    ball_proc(current_target);

    /* 电机指令发送：在调度器任务内执行，避开 DMA ISR 竞争 */
    if (motor_stop_pending) {
        motor_stop_pending = false;
        motor_cmd_pending  = false;
        X_V2_Vel_Control(MOTOR_ADDR, 0, MOTOR_ACCEL, 0.0f, 0);
    } else if (motor_cmd_pending) {
        motor_cmd_pending = false;
        X_V2_Vel_Control(MOTOR_ADDR, motor_cmd_dir, MOTOR_ACCEL, motor_cmd_vel, 0);
    }

    /* 调试打印（需要时） */
    if (dbg_print_flag) {
        dbg_print_flag = false;
        dbg_build_and_send();
    }
}

/* ================================================================
 * 调试打印 — 纯整数格式化，不依赖 MicroLIB 浮点 printf
 *   格式: "M=xx.x p=xx.x v=xx.x tgt=xx.x out=xx.x\r\n"
 *   通过 USART3 (huart3) 发送，即 printf 串口
 * ================================================================ */
static void put_f(char *b, int *pi, float v)
{
    int i = *pi, n;
    if (v >= 0) n = (int)(v * 10.0f + 0.5f);
    else       { b[i++] = '-'; n = (int)(-v * 10.0f + 0.5f); }
    if (n >= 1000) { b[i++] = '0' + n/1000; n %= 1000; }
    if (n >= 100)  { b[i++] = '0' + n/100;  n %= 100; }
    if (n >= 10)   { b[i++] = '0' + n/10;   n %= 10; }
    else           { b[i++] = '0'; }
    b[i++] = '.';
    b[i++] = '0' + n;
    *pi = i;
}

void dbg_build_and_send(void)
{
    char *B = dbg_printf_buf; int p = 0;
    B[p++]='M'; B[p++]='='; put_f(B, &p, motor_angle_deg);
    B[p++]=' '; B[p++]='p'; B[p++]='='; put_f(B, &p, ball_pos_filt);
    B[p++]=' '; B[p++]='v'; B[p++]='='; put_f(B, &p, ball_vel_filt);
    B[p++]=' ';
    B[p++]='t'; B[p++]='g'; B[p++]='t'; B[p++]='=';
    put_f(B, &p, pendulum_target_angle);
    B[p++]=' ';
    B[p++]='o'; B[p++]='u'; B[p++]='t'; B[p++]='=';
    put_f(B, &p, (motor_cmd_dir == 0) ? motor_cmd_vel : -motor_cmd_vel);
    B[p++]='\r'; B[p++]='\n';
    /* 发到 USART3（printf 口），而非相机接收口 USART2 */
    HAL_UART_Transmit(&huart3, (uint8_t *)B, (uint16_t)p, HAL_MAX_DELAY);
}
