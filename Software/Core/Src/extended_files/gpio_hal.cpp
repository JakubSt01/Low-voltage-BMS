/*
 * gpio_hal.cpp
 *
 *  Created on: Jan 15, 2024
 *      Author: Jakub
 */

#include "extended_files/gpio_hal.h"


ISR<GPIO_> GPIO_::ISR_LIST;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	for(uint16_t i=0;i<GPIO_::ISR_LIST.size(); i++){
		GPIO_::ISR_LIST.get(i)->set_switch();
	}
}

GPIO_::GPIO_():
	_switch_state(0){
	  MX_GPIO_Init();
	  ISR_LIST.add(this);

}
void GPIO_::set_bms(action a){

	if(a == on)
		HAL_GPIO_WritePin(POWER_ENABLE_GPIO_Port, POWER_ENABLE_Pin, GPIO_PIN_SET);
	if(a == off){
		HAL_GPIO_WritePin(POWER_ENABLE_GPIO_Port, POWER_ENABLE_Pin, GPIO_PIN_RESET);
		while(1);
	}

}

void GPIO_::set_relay(action a){

	if(a == on) HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_SET);
	if(a == off) HAL_GPIO_WritePin(RELAY_GPIO_Port, RELAY_Pin, GPIO_PIN_RESET);

}

bool GPIO_::display_status(STATUS &status){

	if(status.OVERTEMPERATURE_ERROR == 1)
		HAL_GPIO_WritePin(LED_OTP_GPIO_Port, LED_OTP_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED_OTP_GPIO_Port, LED_OTP_Pin, GPIO_PIN_RESET);

	if(status.UNDERVOLTAGE_ERROR == 1)
		HAL_GPIO_WritePin(LED_UVP_GPIO_Port,LED_UVP_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED_UVP_GPIO_Port,LED_UVP_Pin,GPIO_PIN_RESET);

	if(status.OVERCURRENT_ERROR == 1)
		HAL_GPIO_WritePin(LED_OCP_GPIO_Port,LED_UVP_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED_OCP_GPIO_Port,LED_UVP_Pin,GPIO_PIN_RESET);

	if(status.CHARGE == 1)
		HAL_GPIO_WritePin(LED_CHARGE_GPIO_Port,LED_CHARGE_Pin,GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(LED_CHARGE_GPIO_Port,LED_CHARGE_Pin,GPIO_PIN_RESET);

	if(status.READY == 1){
		HAL_GPIO_WritePin(LED_READY_GPIO_Port, LED_READY_Pin, GPIO_PIN_SET);
		return true;
	}
	//if(HAL_GPIO_ReadPin())
	HAL_GPIO_WritePin(LED_READY_GPIO_Port, LED_READY_Pin, GPIO_PIN_RESET);
	return false;
}
void GPIO_::set_switch(){
   	_switch_state = 1;
   }

uint8_t GPIO_::get_switch(){
   	if(_switch_state == 1 || HAL_GPIO_ReadPin(SWITCH_GPIO_Port, SWITCH_Pin) == GPIO_PIN_SET){
   		_switch_state=0;
		return 1;
	}else
		return 0;
   }
