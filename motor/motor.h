#ifndef motor_h
#define motor_h

#include "main.h"
#include "tim.h"
#include <stdint.h>

/* PWM 和方向引脚配置：名称对应当前 CubeMX 工程。 */
#define MOTOR_PWM_TIMER          htim2
#define MOTOR_LEFT_PWM_CHANNEL   TIM_CHANNEL_1
#define MOTOR_RIGHT_PWM_CHANNEL  TIM_CHANNEL_2
#define MOTOR_SPEED_MAX          100
#define MOTOR_PWM_SCALE          10U

#define MOTOR_LEFT_IN1_PORT      motor_AO1_GPIO_Port
#define MOTOR_LEFT_IN1_PIN       motor_AO1_Pin
#define MOTOR_LEFT_IN2_PORT      motor_AO2_GPIO_Port
#define MOTOR_LEFT_IN2_PIN       motor_AO2_Pin
#define MOTOR_RIGHT_IN1_PORT     motor_BO1_GPIO_Port
#define MOTOR_RIGHT_IN1_PIN      motor_BO1_Pin
#define MOTOR_RIGHT_IN2_PORT     motor_BO2_GPIO_Port
#define MOTOR_RIGHT_IN2_PIN      motor_BO2_Pin

void motor_init(void);
void Left_Dirt(int16_t speed);
void Right_Dirt(int16_t speed);


#endif
