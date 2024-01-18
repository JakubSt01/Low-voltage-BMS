/*
 * bms.h
 *
 *  Created on: Jan 11, 2024
 *      Author: Jakub
 */

#ifndef BMS_H_
#define BMS_H_

#include "main.h"
#include "adc.h"
#include "bms_typedefs.h"
#include "bq76925.h"
#include "cpu.h"
#include "isr_hal.h"

#include <vector> // for array list
#include <math.h>
#include <cstdio>
#include <limits>

using std::vector;




class BMS{
public:


	BMS(ADC_TypeDef *Instance, ADC_HandleTypeDef *hadc, I2C_HandleTypeDef *hi2c);
	// in this case destructor won't be used
	~BMS();


	//---------------------------------------------------------//
	// adc methods

	void start_adc_data_aquisition();
	void perform_adc_conversions();
	bool is_adc_data_ready();
	void read_measurements(vector<VALUES> &data);

	// function that reset BMS status after all conversions and readings performed
	void reset_status();
	void set_status(VALUES &element);

	//init function for setting all data in values to 0
	void VALUES_data_init(VALUES &table, const char * name, const double threshold = std::numeric_limits<double>::quiet_NaN(),  uint8_t *error = nullptr);
	void VALUES_reset();
	void reset_after_error();

	void calculate_input_resistance();
	STATUS _status;



private:



	BQ76925 _bq76925;
	CPU _cpu;

	//adc handlers
	ADC_HandleTypeDef *_hadc;
	ADC_TypeDef *_instance;

	// measured data
	const uint8_t _data_size;
	vector<VALUES> _data;


	//--------------------------------------//
	int _i;
	int _j;

	static constexpr uint8_t _adc_readings_size = 13;
	static constexpr uint16_t _averaging_time = 13;

	// -------------------------------------//

	uint16_t _adc_readings[13];
	uint32_t _conversion_start_time;

	// variables for multiplexed measurements
	STATE_MACHINE _state_machine;
	uint32_t _begin_voltage_settling_time;
	const uint8_t _voltage_settling_time = 80;

	//-------------------------------------------//

	static ISR<BMS> ISR_LIST; // for handling dma interruptions
	static void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);

};





#endif /* BMS_H_ */
