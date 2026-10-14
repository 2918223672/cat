#include "encoder.h"

// 静态变量：对外公开的最终真实速度
volatile float left_speed_cm_s = 0.0f;
volatile float right_speed_cm_s = 0.0f;

// === 文件级静态变量 (供 Encoder_Init 预装基准值) ===
static uint16_t last_left_count = 0;
static uint16_t last_right_count = 0;
static uint32_t last_tick = 0;

void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&ENCODER_LEFT_TIMER, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start(&ENCODER_RIGHT_TIMER, TIM_CHANNEL_ALL);

    __HAL_TIM_SET_COUNTER(&ENCODER_LEFT_TIMER, 0);
    __HAL_TIM_SET_COUNTER(&ENCODER_RIGHT_TIMER, 0);

    // ★ 关键：立即读取当前计数器值作为基准，避免 Encoder_Init 到
    //    首次 Encoder_GetSpeed 之间（LCD 初始化耗时 ~400ms）累积的
    //    噪声脉冲被当作真实速度
    last_left_count  = __HAL_TIM_GET_COUNTER(&ENCODER_LEFT_TIMER);
    last_right_count = __HAL_TIM_GET_COUNTER(&ENCODER_RIGHT_TIMER);
    last_tick = HAL_GetTick();
}

// 读取和计算
void Encoder_GetSpeed(void) {

    uint32_t now_tick = HAL_GetTick();

    // 防止同一毫秒内重复调用（最小间隔 1ms，避免除零）
    if (now_tick == last_tick) return;
    float dt = (now_tick - last_tick) * 0.001f;  // 转换为秒
    last_tick = now_tick;

    // 当前脉冲数
    uint16_t current_left = __HAL_TIM_GET_COUNTER(&ENCODER_LEFT_TIMER);
    uint16_t current_right = __HAL_TIM_GET_COUNTER(&ENCODER_RIGHT_TIMER);

    // 脉冲差值（uint16 减法 → int 提升 → int16 截断，自动处理 16 位翻转）
    int16_t delta_left = ENCODER_LEFT_DIRECTION * (int16_t)(current_left - last_left_count);
    int16_t delta_right = ENCODER_RIGHT_DIRECTION * (int16_t)(current_right - last_right_count);

    // 更新 last，供下次用
    last_left_count = current_left;
    last_right_count = current_right;

    // ★ 噪声死区：静止时 EMI 抖动通常 ±1~3 个脉冲，直接清零
    if (delta_left  >= -ENCODER_DEAD_ZONE && delta_left  <= ENCODER_DEAD_ZONE) delta_left  = 0;
    if (delta_right >= -ENCODER_DEAD_ZONE && delta_right <= ENCODER_DEAD_ZONE) delta_right = 0;

    // 轮子周长 (cm)
    float wheel_circumference = PI * WHEEL_DIAMETER;

    if (ENCODER_PPR != 0.0f && dt > 0.0f) {
        // 速度(cm/s) = (脉冲差值 / 一圈总脉冲数) * 轮子周长(cm) / 实际时间(s)
        float raw_left  = (delta_left  / ENCODER_PPR) * wheel_circumference / dt;
        float raw_right = (delta_right / ENCODER_PPR) * wheel_circumference / dt;

        // 前进=正速度，与电机输出正方向对齐
        left_speed_cm_s  = (ENCODER_SPEED_ALPHA * raw_left) + ((1.0f - ENCODER_SPEED_ALPHA) * left_speed_cm_s);
        right_speed_cm_s = (ENCODER_SPEED_ALPHA * raw_right) + ((1.0f - ENCODER_SPEED_ALPHA) * right_speed_cm_s);
    } else {
        left_speed_cm_s = 0.0f;
        right_speed_cm_s = 0.0f;
    }
}
