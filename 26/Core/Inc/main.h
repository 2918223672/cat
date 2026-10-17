/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STBY_Pin GPIO_PIN_5
#define STBY_GPIO_Port GPIOA
#define ENC1_A_Pin GPIO_PIN_6
#define ENC1_A_GPIO_Port GPIOA
#define ENC1_B_Pin GPIO_PIN_7
#define ENC1_B_GPIO_Port GPIOA
#define motor_AO1_Pin GPIO_PIN_4
#define motor_AO1_GPIO_Port GPIOC
#define Lcd_D_C_Pin GPIO_PIN_5
#define Lcd_D_C_GPIO_Port GPIOC
#define motor_AO2_Pin GPIO_PIN_0
#define motor_AO2_GPIO_Port GPIOB
#define Lcd_BLK_Pin GPIO_PIN_1
#define Lcd_BLK_GPIO_Port GPIOB
#define ZDT_COM_L_Pin GPIO_PIN_11
#define ZDT_COM_L_GPIO_Port GPIOE
#define ZDT_EN_L_Pin GPIO_PIN_12
#define ZDT_EN_L_GPIO_Port GPIOE
#define ZDT_COM_H_Pin GPIO_PIN_13
#define ZDT_COM_H_GPIO_Port GPIOE
#define ZDT_EN_H_Pin GPIO_PIN_14
#define ZDT_EN_H_GPIO_Port GPIOE
#define Lcd_CS_Pin GPIO_PIN_12
#define Lcd_CS_GPIO_Port GPIOB
#define ZK_CS_Pin GPIO_PIN_8
#define ZK_CS_GPIO_Port GPIOD
#define motor_BO1_Pin GPIO_PIN_10
#define motor_BO1_GPIO_Port GPIOD
#define motor_BO2_Pin GPIO_PIN_11
#define motor_BO2_GPIO_Port GPIOD
#define ENC2_A_Pin GPIO_PIN_12
#define ENC2_A_GPIO_Port GPIOD
#define ENC2_B_Pin GPIO_PIN_13
#define ENC2_B_GPIO_Port GPIOD
#define BUZZER_Pin GPIO_PIN_15
#define BUZZER_GPIO_Port GPIOD
#define track_bit2_Pin GPIO_PIN_8
#define track_bit2_GPIO_Port GPIOC
#define track_bit1_Pin GPIO_PIN_9
#define track_bit1_GPIO_Port GPIOC
#define key_nev_black_Pin GPIO_PIN_9
#define key_nev_black_GPIO_Port GPIOA
#define key_nev_white_Pin GPIO_PIN_10
#define key_nev_white_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
