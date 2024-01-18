/*
 * cpu.h
 *
 *  Created on: Jan 16, 2024
 *      Author: Jakub
 */

#ifndef INC_EXTENDED_FILES_CPU_H_
#define INC_EXTENDED_FILES_CPU_H_

#include "main.h"
#include <vector>
#include <math.h>
#include "bms_typedefs.h"


class CPU{

public:
	CPU();

	void convert_value_into_voltage(VALUES &element);
	void calculate_internal_temperature(VALUES &element);
	void calculate_internal_reference_voltage(VALUES &element);
	void calculate_thermistor_teperature(VALUES &element);

private:

	static constexpr double _internal_ref_voltage = 1.2 ;
	static constexpr uint16_t _max_adc_value = 4095;


	const double V25 = 1.43;
	const double dV = -0.0043;


	// thermistor parameters
	// Steinhart-Hart coefficients for the thermistor

	static constexpr double _A = 0.0018249730;
	static constexpr double _B = 0.0001505593;
	static constexpr double _C = 0.0000003993;
	static constexpr double _R = 10000;

	double Rtherm;



	double _ref_voltage; // changed by calculate_internal_reference_voltage method

};



#endif /* INC_EXTENDED_FILES_CPU_H_ */
