/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

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
#define CURRENT_ALERT_Pin GPIO_PIN_13
#define CURRENT_ALERT_GPIO_Port GPIOC
#define LED_READY_Pin GPIO_PIN_14
#define LED_READY_GPIO_Port GPIOC
#define LED_OTP_Pin GPIO_PIN_15
#define LED_OTP_GPIO_Port GPIOC
#define HIGH_CURRENT_Pin GPIO_PIN_0
#define HIGH_CURRENT_GPIO_Port GPIOC
#define LOW_CURRENT_Pin GPIO_PIN_1
#define LOW_CURRENT_GPIO_Port GPIOC
#define CELL_VOLTAGES_Pin GPIO_PIN_2
#define CELL_VOLTAGES_GPIO_Port GPIOC
#define LED_CHARGE_Pin GPIO_PIN_0
#define LED_CHARGE_GPIO_Port GPIOA
#define LED_UVP_Pin GPIO_PIN_1
#define LED_UVP_GPIO_Port GPIOA
#define LED_OCP_Pin GPIO_PIN_2
#define LED_OCP_GPIO_Port GPIOA
#define T8_Pin GPIO_PIN_3
#define T8_GPIO_Port GPIOA
#define T7_Pin GPIO_PIN_4
#define T7_GPIO_Port GPIOA
#define T6_Pin GPIO_PIN_5
#define T6_GPIO_Port GPIOA
#define T5_Pin GPIO_PIN_6
#define T5_GPIO_Port GPIOA
#define T4_Pin GPIO_PIN_7
#define T4_GPIO_Port GPIOA
#define T3_Pin GPIO_PIN_4
#define T3_GPIO_Port GPIOC
#define T2_Pin GPIO_PIN_5
#define T2_GPIO_Port GPIOC
#define T1_Pin GPIO_PIN_0
#define T1_GPIO_Port GPIOB
#define SWITCH_Pin GPIO_PIN_12
#define SWITCH_GPIO_Port GPIOB
#define SWITCH_EXTI_IRQn EXTI15_10_IRQn
#define POWER_ENABLE_Pin GPIO_PIN_13
#define POWER_ENABLE_GPIO_Port GPIOB
#define RELAY_Pin GPIO_PIN_9
#define RELAY_GPIO_Port GPIOA
#define D0_Pin GPIO_PIN_10
#define D0_GPIO_Port GPIOA
#define D1_Pin GPIO_PIN_11
#define D1_GPIO_Port GPIOA
#define D2_Pin GPIO_PIN_12
#define D2_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
