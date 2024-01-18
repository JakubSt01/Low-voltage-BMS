/*
 * cpu.cpp
 *
 *  Created on: Jan 16, 2024
 *      Author: Jakub
 */

#include "extended_files/cpu.h"

CPU::CPU():
Rtherm(0),
_ref_voltage(3.3){ // initializing default with value 3.3 "in case"

}

void CPU::convert_value_into_voltage(VALUES &element){
	element.value *= _ref_voltage/ _max_adc_value;
}

void CPU::calculate_internal_temperature(VALUES &element){
	element.value = (element.value - V25 )/dV + 25.0 ; // value is very bad as datasheet says - between procesors it could bo up to 45 offset
}
void CPU::calculate_internal_reference_voltage(VALUES &element){
	_ref_voltage = element.value =_max_adc_value*_internal_ref_voltage/element.value;
}

void CPU::calculate_thermistor_teperature(VALUES &element){
	Rtherm = (_R *element.value)/(_ref_voltage - element.value);
	element.value =  1.0 / (_A + _B * log(Rtherm) + _C * pow(log(Rtherm), 3)) - 273.15;

}


