#include "motor.h"


//调度器调用初始化
void motor_init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // 对应 Left_Dirt
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // 对应 Right_Dirt

    // 显式停止电机，确保上电时方向引脚和 PWM 占空比均为 0
    Left_Dirt(0);
    Right_Dirt(0);
}

uint16_t pwm_duty_1;
uint16_t pwm_duty_2;

void Left_Dirt(int16_t speed)
{
    if(speed > 100) speed = 100;
    if(speed < -100) speed = -100;

    pwm_duty_1 = (speed >= 0) ? speed : -speed;

    if(speed > 0) {
        /* 正转 (前进) — 后驱底盘 */
        HAL_GPIO_WritePin(motor_AO1_GPIO_Port, motor_AO1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor_AO2_GPIO_Port, motor_AO2_Pin, GPIO_PIN_RESET);
    }
    else if(speed < 0) {
        /* 反转 (后退) */
        HAL_GPIO_WritePin(motor_AO1_GPIO_Port, motor_AO1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor_AO2_GPIO_Port, motor_AO2_Pin, GPIO_PIN_SET);
    }
    else {
        /* 停止 (刹车/滑行) */
        HAL_GPIO_WritePin(motor_AO1_GPIO_Port, motor_AO1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor_AO2_GPIO_Port, motor_AO2_Pin, GPIO_PIN_RESET);
    }

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pwm_duty_1 * 10);
}

void Right_Dirt(int16_t speed)
{
    if(speed > 100) speed = 100;
    if(speed < -100) speed = -100;

    pwm_duty_2 = (speed >= 0) ? speed : -speed;

    if(speed > 0) {
        /* 正转 (前进) — 后驱底盘 */
        HAL_GPIO_WritePin(motor_BO1_GPIO_Port, motor_BO1_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(motor_BO2_GPIO_Port, motor_BO2_Pin, GPIO_PIN_RESET);
    }
    else if(speed < 0) {
        /* 反转 (后退) */
        HAL_GPIO_WritePin(motor_BO1_GPIO_Port, motor_BO1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor_BO2_GPIO_Port, motor_BO2_Pin, GPIO_PIN_SET);
    }
    else {
        /* 停止 (刹车/滑行) */
        HAL_GPIO_WritePin(motor_BO1_GPIO_Port, motor_BO1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(motor_BO2_GPIO_Port, motor_BO2_Pin, GPIO_PIN_RESET);
    }

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pwm_duty_2 * 10);
}


