#ifndef __CLOUD_H__
#define __CLOUD_H__

#include "Myconfig.h"   /* 包含 usart.h → cam_rx_buff/cam_frame_flag/cam_rx_count */
#include <math.h>

/* ========== Alpha-Beta 滤波器参数 (球位置 & 速度估计) ==========
 *  预测:  pos += vel * dt
 *  更新:  r = meas - pos
 *         pos += A * r              (α = 位置增益, 0~1, 大=快速响应 小=强滤波)
 *         vel += B * r / dt         (β = 速度增益, 0~1, 大=快速响应 小=强滤波)
 *
 *  稳态等价于卡尔曼 — 对匀速模型+固定噪声，卡尔曼增益收敛后就是 α,β 定值
 *  v2: α 从 0.15→0.08 / β 从 0.005→0.002, 位置传感器噪声大, 加强滤波
 */
#define AB_DT             0.01f   /* 时间步长 (s) = 10ms */
#define AB_ALPHA          0.08f   /* 位置增益 — 降低以减少传感器噪声 (原0.15) */
#define AB_BETA           0.002f  /* 速度增益 — 降低以平滑速度估计 (原0.005) */

/* Alpha-Beta 滤波器状态 (2态: 位置 + 速度, 仅 2 float) */
typedef struct {
    float pos;      /* 滤波位置 (mm) */
    float vel;      /* 滤波速度 (mm/s) */
    uint8_t inited; /* 初始化标志 */
} AlphaBeta_t;

/* ========== 外环：球位置 PID → 目标摆杆角度 ==========
 *  v3: 降低 KP 减缓位置响应防止过冲，增大 KV 加强速度制动，
 *      加入微小 KI 补偿平台机械倾斜造成的系统性偏置
 *
 *  综合等效参数 (ANGLE_KP=2.5):
 *    有效位置增益 = 2.5 × 0.03 = 0.075 RPM/mm   (pos=40mm → 3.0 RPM)
 *    有效速度阻尼 = 2.5 × 0.12 = 0.30 RPM/(mm/s) (vel=50mm/s → 15 RPM制动)
 */
#define PENDULUM_MAX_DEG  8.0f     /* 摆杆最大倾斜角度 (°) */
#define POS_KP            0.03f    /* 位置环 Kp */
#define POS_KI            0.003f   /* 位置环 Ki — 补偿平台倾斜偏置 */
/* POS_KD 已移除: e=xd-x, ė=-v → -Kd*ė = +Kd*v, 与 -Kv*v 相减削弱阻尼
 * 切换目标时还会产生尖峰。仅用 -POS_KV*vel_filt 做测量微分阻尼。 */
#define POS_KV            0.12f    /* 球速度阻尼 (已滤波, 相当于无kick的微分) */
/* 死区: 位置 AND 速度均小于阈值才进入保持态; 带迟滞防止开关振荡 */
#define POS_DEADZONE_MM       2.0f  /* 进入保持态: |pos_err| < 2mm */
#define POS_DEADZONE_EXIT_MM  3.5f  /* 退出保持态: |pos_err| > 3.5mm (迟滞) */
#define POS_HOLD_VEL_MM      15.0f  /* 进入保持态: |vel| < 15mm/s */
#define POS_MAX_INTEGRAL  3.0f     /* 位置环积分限幅 */

/* ========== 内环：摆杆角度 PID → 电机转速 ==========
 *  v3: ANGLE_KP 4.0 → 2.5, ANGLE_MAX_OUT 40 → 20 RPM
 *  降低最大转速至 20 RPM (对应脉冲频率 ~1067 Hz @ 200步/圈 16细分)
 *  避免饱和后的大过冲, 同时保留足够响应能力
 */
#define ANGLE_KP          2.5f     /* 角度环 Kp (v2=4.0 → 降低) */
#define ANGLE_KI          0.0f     /* 角度环 Ki — 保持清零 */
#define ANGLE_KD          0.0f     /* 角度环 Kd — 保持清零 */
#define ANGLE_MAX_OUT     20.0f    /* 内环输出最大转速 RPM (v2=40 → 20 RPM) */
#define ANGLE_MAX_INTEGRAL 5.0f    /* 角度环积分限幅 */

/* ========== 电机参数 ========== */
#define MOTOR_ADDR         1       /* ZDT 电机地址 */
#define MOTOR_MAX_SPEED    20.0f   /* 电机最大转速 (RPM) — 与 ANGLE_MAX_OUT 保持一致 */
#define MOTOR_ACCEL        200     /* 电机加减速度 (RPM/s) */
#define MOTOR_MAX_CURRENT  2000    /* 电机限电流 (mA) */
#define GEAR_RATIO         5.0f    /* 电机:摆杆 减速比（保留，速度模式下不直接使用） */

/* ========== dx → 脉冲频率 完整链路参数 ==========
 *  步进电机脉冲频率换算:
 *    freq(Hz) = RPM × STEPS_PER_REV × MICROSTEP / 60
 *
 *  典型 X42S / NEMA17 (1.8°/步):
 *    STEPS_PER_REV = 200
 *    MICROSTEP     = 16  (出厂默认)
 *    PULSES_PER_REV= 3200
 *
 *  换算表:
 *     5 RPM →  267 Hz
 *    10 RPM →  533 Hz
 *    15 RPM →  800 Hz
 *    20 RPM → 1067 Hz  ← 当前上限
 */
#define MOTOR_STEPS_PER_REV  200           /* 步/圈 (1.8°步进电机) */
#define MOTOR_MICROSTEP      16            /* 细分数，与驱动器设置一致 */
#define MOTOR_PULSES_PER_REV (MOTOR_STEPS_PER_REV * MOTOR_MICROSTEP)  /* 3200 脉冲/圈 */
/* RPM → 脉冲频率 (Hz) */
#define RPM_TO_HZ(rpm)   ((rpm) * (float)MOTOR_PULSES_PER_REV / 60.0f)

/* ========== 球位置 ========== */
extern float ball_current_pos;       /* 球位置原始测量值 (mm)，USART2 更新 */
extern float ball_pos_filt;          /* Alpha-Beta 滤波后的球位置 (mm) */
extern float ball_vel_filt;          /* Alpha-Beta 滤波后的球速度 (mm/s) */
extern AlphaBeta_t ball_ab;          /* Alpha-Beta 滤波器句柄 */

/* ========== 摆杆角度 ========== */
extern float pendulum_target_angle;  /* 外环 PID 输出的目标倾斜角度 (°) */
extern float motor_angle_deg;        /* 电机累计角度 (°)，速度积分 */

/* ========== 调试变量 ========== */
extern float     dbg_dx_raw;
extern float     dbg_dx_filt;
extern float     dbg_dx_max;
extern uint16_t  dbg_call_cnt;
extern uint16_t  dbg_dx_small;
extern uint16_t  dbg_dx_medium;
extern uint16_t  dbg_dx_large;
extern uint16_t  dbg_pos_jump;
extern uint16_t  dbg_dir_flip;
extern uint16_t  dbg_deadzone;
extern uint16_t  dbg_saturate;
extern uint16_t  dbg_angle_saturate;

/* ========== 调试打印 ========== */
#define DBG_PRINT_INTERVAL 100       /* 每 100 次调用 (1秒) 打印一次统计 */
extern volatile bool dbg_print_flag;
extern char dbg_printf_buf[256];

/* ========== 电机指令延迟发送（在调度器任务 zdt_test_proc 里执行，避开 ISR DMA 冲突） ========== */
extern volatile bool motor_cmd_pending;  /* true → 发速度指令 */
extern volatile bool motor_stop_pending; /* true → 发停止指令 (0 RPM) */
extern uint8_t  motor_cmd_dir;
extern float    motor_cmd_vel;

/* ========== 视觉帧新鲜标志 ========== */
/* cloud_proc_cam_data() 置位, ab_update() 消耗
 * 保证 Alpha-Beta 滤波器只在收到新视觉帧时做测量更新,
 * 控制环的其他帧只做预测步骤, 避免重复观测 */
extern volatile bool ball_pos_updated;

/* ========== dx → 脉冲频率 打包结构体 ========== */
/**
 * 从球位置误差 (dx) 到步进电机脉冲频率的完整计算结果
 *   dx(mm) → 外环PID → target_angle(°) → 内环P → rpm → pulse_freq(Hz)
 */
typedef struct {
    float target_angle;  /* 外环 PID 输出的目标倾斜角度 (°) */
    float rpm;           /* 内环 P 输出的电机转速，带符号 (RPM, 正=CW) */
    float pulse_freq;    /* 对应的步进脉冲频率，带符号 (Hz, 正=CW) */
    uint8_t dir;         /* 电机方向: 0=CW, 1=CCW */
    float abs_rpm;       /* |rpm|，直接传给 X_V2_Vel_Control */
} MotorCmd_t;

/* ========== 目标位置（外部设置，调度器自动读取）========== */
/* 在任意地方写 g_ball_target_pos = xxx 即可改变小球目标位置 (mm) */
extern float g_ball_target_pos;

/* ========== 函数声明 ========== */
void ab_init(AlphaBeta_t *ab);
void ab_update(AlphaBeta_t *ab, float meas);
void cloud_proc_cam_data(void);      /* 解析 USART2 相机数据，更新 ball_current_pos */
void ball_proc(float ball_target_pos);

/**
 * @brief  dx → 步进脉冲频率 完整链路打包函数
 *
 * @param  dx           球位置误差 = target_pos - current_pos (mm)
 * @param  ball_vel     Alpha-Beta 滤波后球速度 (mm/s)
 * @param  actual_angle 角度传感器实际倾角 (°)；无传感器时传 0.0f
 * @param  pos_integral 外环积分状态 (调用方维护, 传指针)
 * @param  pos_err_last 上一帧位置误差 (调用方维护, 传指针)
 * @param  cmd          [输出] 计算结果，含角度/RPM/脉冲频率/方向
 */
void dx_to_pulse(float dx, float ball_vel, float actual_angle,
                 float *pos_integral, float *pos_err_last,
                 MotorCmd_t *cmd);

/**
 * @brief  平衡控制主任务（带目标位置参数）
 *
 * @param  ruler  小球目标位置 (mm，0 = 平台中心)
 *                正值 = 偏右，负值 = 偏左
 *
 * 直接调用示例:  zdt_test_proc(20.0f);   // 让球移到右侧 20mm
 * 调度器自动调用时读取 g_ball_target_pos，效果相同
 */
void zdt_test_proc(float ruler);
void dbg_build_and_send(void);

#endif
