/*
 * usart_hal.h
 *
 *  Created on: Jan 15, 2024
 *      Author: Jakub
 */

#ifndef INC_EXTENDED_FILES_USART_HAL_H_
#define INC_EXTENDED_FILES_USART_HAL_H_

#include "main.h"
#include "usart.h"
#include "extended_files\bms.h"
#include<cstdio>
using std::vector;

class USART{
public:
	USART(UART_HandleTypeDef *huart);

	void write(vector<VALUES> &data,uint32_t time);

private:
	UART_HandleTypeDef *_huart;
	uint8_t _buffer[1000];
	uint32_t _len;
	bool _is_ready;
	uint16_t _i;

};



#endif /* INC_EXTENDED_FILES_USART_HAL_H_ */
