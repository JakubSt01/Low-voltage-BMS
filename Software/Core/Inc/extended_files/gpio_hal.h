/*
 * gpio_hal.h
 *
 *  Created on: Jan 15, 2024
 *      Author: Jakub
 */

#ifndef INC_EXTENDED_FILES_GPIO_HAL_H_
#define INC_EXTENDED_FILES_GPIO_HAL_H_
#include "main.h"
#include "gpio.h"
#include "extended_files/BMS.h"
#include "extended_files/isr_hal.h"

class GPIO_{
public:
	GPIO_();

	enum action{on, off};

	void set_bms(action a);
    void set_relay(action a);
	bool display_status(STATUS &status);

    void set_switch();
    uint8_t get_switch();

	static ISR<GPIO_> ISR_LIST;

private:
	uint8_t _switch_state;

};


#endif /* INC_EXTENDED_FILES_GPIO_HAL_H_ */
