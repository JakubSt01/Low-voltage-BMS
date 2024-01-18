/*
 * uart_hal.cpp
 *
 *  Created on: Jan 15, 2024
 *      Author: Jakub
 */

#include "extended_files/usart_hal.h"



USART::USART(UART_HandleTypeDef *huart):
	_huart(huart),
	_len(0),
	_is_ready(true),
	_i(0){

	 MX_USART1_UART_Init();

}


void USART::write(vector<VALUES> &data,uint32_t time){
	_len=0;
	for(_i=0;_i<data.size();_i++){
		_len+=sprintf((char*)&_buffer[_len],"%s %.3f s=%d \n\r",data[_i].name, data[_i].value, data[_i].number_of_samples);
	}
	_len+=sprintf((char*)&_buffer[_len],"remaining time = %d \n\r", time/1000);
	 HAL_UART_Transmit_DMA(_huart, _buffer, _len);
}

