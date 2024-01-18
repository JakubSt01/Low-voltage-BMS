/*
 * main.cpp
 *
 *  Created on: Jan 11, 2024
 *      Author: Jakub
 */

#include "main.h"
#include "can.h"
#include "dma.h"

#include "extended_files\bms.h"
#include "extended_files\usart_hal.h"
#include "extended_files\gpio_hal.h"
#include "gpio.h"
#include <vector>
#include <stdio.h>

void SystemClock_Config(void);
uint16_t vref;


int main(void)
{

  HAL_Init();


  SystemClock_Config();




  MX_DMA_Init();
  MX_CAN1_Init();

  GPIO_ gpio{};
  //enabling  parallel power switch
  gpio.set_bms(gpio.on);

// initializing classes
  BMS bms(ADC1, &hadc1, &hi2c2);
  USART usart(&huart1);


  // override all safety features on start while pushing the button

  do{
	  gpio.set_relay(gpio.on);
  }while (gpio.get_switch() == 1);

  vector<VALUES> data{};

  // time constants and variables
  uint32_t previous_tick_uart{0};
  const  uint32_t uart_time_period{500};

  uint32_t previous_tick_gpio{0};
  const  uint32_t gpio_time_period{10};

  uint32_t shutdown_timer{0};
  const uint32_t shutdown_time_limit{60000};

  uint32_t shutdown_switch_timer{0};
  const uint32_t shutdown_switch_time_limit{3000};

  bms.start_adc_data_aquisition();

  while (1)
  {
	if(bms.is_adc_data_ready()){
	  //reading bms measurements
	  bms.read_measurements(data);

	  //sending data to uart every 500ms or when error ocured
	  if(HAL_GetTick() - previous_tick_uart > uart_time_period || bms._status.READY == 0){
			usart.write(data,shutdown_time_limit + shutdown_timer - HAL_GetTick() );
			previous_tick_uart=HAL_GetTick();
	  }

	  // setting diodes to show actual status and catching errors if they appear
	  if( !gpio.display_status(bms._status) ) // returns true when _status is ready
	  {
		  //handling errors - while true loop
		  while(1){
			  // output is turned off immidietly
			  gpio.set_relay(gpio.off);
			  if(gpio.get_switch() == 1) // when button is pushed - bms is resetting itself by switching off power supply - you must push button twice to turn it on again
			  {
				  gpio.set_bms(gpio.off);
			  }
		  }
	  }

	  bms.start_adc_data_aquisition();
	}

	if(HAL_GetTick() - previous_tick_gpio > gpio_time_period){ //  every gpio_time_period if will return true
	  // turning on relay and reset timer
	  if(gpio.get_switch() == 1){ // waiting for pushbutton
		  gpio.set_bms(gpio.on);
		  gpio.set_relay(gpio.on);
		  shutdown_timer = HAL_GetTick();
	  }else{
		  shutdown_switch_timer = HAL_GetTick();
	  }

	  //turning off relay after timer or after 3 seconds when button is pushed
	  if(HAL_GetTick() - shutdown_timer > shutdown_time_limit ||
			  HAL_GetTick() - shutdown_switch_timer > shutdown_switch_time_limit){
		  gpio.set_relay(gpio.off);
		  gpio.set_bms(gpio.off);
	  }
	  previous_tick_gpio = HAL_GetTick();
	}
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV4;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL10;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV4;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */




