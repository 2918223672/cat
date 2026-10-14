#include "motor.h"


//调度器调用初始化
void motor_init(void)
{
    HAL_TIM_PWM_Start(&MOTOR_PWM_TIMER, MOTOR_LEFT_PWM_CHANNEL);
    HAL_TIM_PWM_Start(&MOTOR_PWM_TIMER, MOTOR_RIGHT_PWM_CHANNEL);

    // 显式停止电机，确保上电时方向引脚和 PWM 占空比均为 0
    Left_Dirt(0);
    Right_Dirt(0);
}

uint16_t pwm_duty_1;
uint16_t pwm_duty_2;

void Left_Dirt(int16_t speed)
{
    if(speed > MOTOR_SPEED_MAX) speed = MOTOR_SPEED_MAX;
    if(speed < -MOTOR_SPEED_MAX) speed = -MOTOR_SPEED_MAX;

    pwm_duty_1 = (speed >= 0) ? speed : -speed;

    if(speed > 0) {
        /* 正转 (前进) — 后驱底盘 */
        HAL_GPIO_WritePin(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN, GPIO_PIN_RESET);
    }
    else if(speed < 0) {
        /* 反转 (后退) */
        HAL_GPIO_WritePin(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN, GPIO_PIN_SET);
    }
    else {
        /* 停止 (刹车/滑行) */
        HAL_GPIO_WritePin(MOTOR_LEFT_IN1_PORT, MOTOR_LEFT_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_LEFT_IN2_PORT, MOTOR_LEFT_IN2_PIN, GPIO_PIN_RESET);
    }

    __HAL_TIM_SET_COMPARE(&MOTOR_PWM_TIMER, MOTOR_LEFT_PWM_CHANNEL, pwm_duty_1 * MOTOR_PWM_SCALE);
}

void Right_Dirt(int16_t speed)
{
    if(speed > MOTOR_SPEED_MAX) speed = MOTOR_SPEED_MAX;
    if(speed < -MOTOR_SPEED_MAX) speed = -MOTOR_SPEED_MAX;

    pwm_duty_2 = (speed >= 0) ? speed : -speed;

    if(speed > 0) {
        /* 正转 (前进) — 后驱底盘 */
        HAL_GPIO_WritePin(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN, GPIO_PIN_RESET);
    }
    else if(speed < 0) {
        /* 反转 (后退) */
        HAL_GPIO_WritePin(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN, GPIO_PIN_SET);
    }
    else {
        /* 停止 (刹车/滑行) */
        HAL_GPIO_WritePin(MOTOR_RIGHT_IN1_PORT, MOTOR_RIGHT_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_RIGHT_IN2_PORT, MOTOR_RIGHT_IN2_PIN, GPIO_PIN_RESET);
    }

    __HAL_TIM_SET_COMPARE(&MOTOR_PWM_TIMER, MOTOR_RIGHT_PWM_CHANNEL, pwm_duty_2 * MOTOR_PWM_SCALE);
}


