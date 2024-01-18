/*
 * bms.cpp
 *
 *  Created on: Jan 11, 2024
 *      Author: Jakub
 */
#include "extended_files\bms.h"

ISR<BMS>BMS::ISR_LIST;

void BMS::HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
	for(uint16_t i=0;i<ISR_LIST.size(); i++){
		BMS *handle = ISR_LIST.get(i);
		if(handle->_hadc == hadc ){
			handle->perform_adc_conversions();
		}
	}
}

BMS::BMS(ADC_TypeDef *Instance, ADC_HandleTypeDef *hadc, I2C_HandleTypeDef *hi2c):
	// creating classes
	_status({1,0,0,0,0,0}),
	_bq76925(hi2c),
	_cpu(),
	// adc instance
	_hadc(hadc),
	_instance(Instance),
	// creatind data table
	_data_size(17),
	_data(_data_size),
	//iterators
	_i(0),
	_j(0),
	_state_machine(start),
	_begin_voltage_settling_time(0){

#if USE_HAL_ADC_REGISTER_CALLBACKS != 1
#error "ADC register callbacks must be enabled"
#endif
	MX_ADC1_Init();
	_hadc-> ConvCpltCallback = HAL_ADC_ConvCpltCallback;
	HAL_ADCEx_Calibration_Start(_hadc);

	VALUES_data_init(_data[0],"Vref:"); 					//Vref
	VALUES_data_init(_data[1],"Cpu internal temperature:", 50, &_status.OVERTEMPERATURE_ERROR); //temp internal cpu
	VALUES_data_init(_data[2],"T1", 30, &_status.OVERTEMPERATURE_ERROR); 						//T1
	VALUES_data_init(_data[3],"T2", 30, &_status.OVERTEMPERATURE_ERROR); 						//T2
	VALUES_data_init(_data[4],"T3", 30, &_status.OVERTEMPERATURE_ERROR); 						//T3
	VALUES_data_init(_data[5],"T4", 30, &_status.OVERTEMPERATURE_ERROR); 						//T4
	VALUES_data_init(_data[6],"T5", 30, &_status.OVERTEMPERATURE_ERROR); 						//T5
	VALUES_data_init(_data[7],"T6", 30, &_status.OVERTEMPERATURE_ERROR); 						//T6
	VALUES_data_init(_data[8],"T7", 30, &_status.OVERTEMPERATURE_ERROR); 						//T7
	VALUES_data_init(_data[9],"T8", 30, &_status.OVERTEMPERATURE_ERROR); 						//T8
	VALUES_data_init(_data[10],"High current:", 130, &_status.OVERCURRENT_ERROR);			//High current
	VALUES_data_init(_data[11],"Low current");				//Low current
	VALUES_data_init(_data[12],"Internal bq refference");  //internal temp bq
	VALUES_data_init(_data[13],"V1",2.5, &_status.UNDERVOLTAGE_ERROR);						//V1
	VALUES_data_init(_data[14],"V2",2.5, &_status.UNDERVOLTAGE_ERROR);						//V2
	VALUES_data_init(_data[15],"V3",2.5, &_status.UNDERVOLTAGE_ERROR);						//V3
	VALUES_data_init(_data[16],"Internal bq temperature", 70, &_status.OVERTEMPERATURE_ERROR);  //internal temp bq

	ISR_LIST.add(this);
}

void BMS::VALUES_data_init(VALUES &table, const char * name, const double threshold, uint8_t * error){
	table.value = 0;
	table.number_of_samples = 0;
	if(std::isnan(threshold))
		sprintf((char*)table.name, "%s", name);
	else{
		sprintf((char*)table.name, "%s th(%.2f)", name,threshold);
		table.threshold = threshold;
	}
	table.error = error;
}

void BMS::VALUES_reset(){
	for(_i = 0 ;_i<_adc_readings_size-1;_i++){
		_data[_i].value = 0;
		_data[_i].number_of_samples = 0;
	}
	if(_state_machine == aquisition){
		_data[_adc_readings_size-1 +_j].value = 0;
		_data[_adc_readings_size-1 +_j].number_of_samples = 0;
	}
}

void BMS::start_adc_data_aquisition(){
	_status.AVERAGING_DONE = 0; // reseting state of AVERAGING
	if( HAL_GetTick() - _begin_voltage_settling_time  > _voltage_settling_time){
		_state_machine = aquisition;
	}
	VALUES_reset(); // clearing values table and number of samples (for bq only for selected value)
	_conversion_start_time = HAL_GetTick();
	HAL_ADC_Start_DMA(_hadc, (uint32_t*)_adc_readings, _adc_readings_size);

}

void BMS::perform_adc_conversions(){
	// function that implements moving average method
	if(HAL_GetTick() - _conversion_start_time < _averaging_time){
		for(_i = 0 ;_i<_adc_readings_size-1;_i++){
					_data[_i].value += _adc_readings[_i]; // uint32_t contains moner tham 1M sum of 12b samples
					_data[_i].number_of_samples++;
			}
		if(_state_machine == aquisition){
			_data[_adc_readings_size-1+_j].value += _adc_readings[_adc_readings_size-1]; // uint32_t contains moner tham 1M sum of 12b samples
			_data[_adc_readings_size-1+_j].number_of_samples++;
		}

		HAL_ADC_Start_DMA(_hadc, (uint32_t*)_adc_readings, _adc_readings_size);

	}else{
		if(_state_machine == aquisition) _state_machine=converting;
		_status.AVERAGING_DONE = 1;
	}
}

bool BMS::is_adc_data_ready(){
	if( _status.AVERAGING_DONE == 1 ){

		for(_i=0;_i<_adc_readings_size-1;_i++)
			_data[_i].value/=_data[_i].number_of_samples;

		// first of all find and calculate reference voltage - else vref is equal to 3300mV
		_cpu.calculate_internal_reference_voltage(_data[0]);

		//converting all data into voltage (without multiplexed bq data
		for(_i=1;_i<_adc_readings_size-1;_i++)
			_cpu.convert_value_into_voltage(_data[_i]);// all conversions

		// calculating internal cpu temperature
		_cpu.calculate_internal_temperature(_data[1]);
		set_status(_data[1]);

		// calculating thermistors temperatures
		for(_i=2;_i<10;_i++)
		{
			_cpu.calculate_thermistor_teperature(_data[_i]);
			set_status(_data[_i]);
		}

		//calculating high current
		_data[10].value = (_data[10].value*1.51515151 - 2.5) * 0.013; // offset 2.5V cV/dA = 0.013
		set_status(_data[10]);

		// multiplexed conversions and selecting next measured parameter (doing it here because of delay between sending data and settling voltage at output bq pin)

		if(_state_machine == converting){

			_data[_adc_readings_size-1+_j].value/=_data[_adc_readings_size-1+_j].number_of_samples;

			_cpu.convert_value_into_voltage(_data[_adc_readings_size-1 + _j]);

			switch(_j){
			default:
					_j=0;
			case 0:
				_bq76925.set(_bq76925.CELL_CTL, _bq76925.VCOUT_SEL_VCn  + _bq76925.CELL_SEL_VC1); // next measured parameter
			// calculating bq internal reference voltage - measured value is 0.5 bq vref
				_bq76925.calculate_internal_reference_voltage(_data[12]);
				break;
			case 1:
				_bq76925.set(_bq76925.CELL_CTL, _bq76925.VCOUT_SEL_VCn  + _bq76925.CELL_SEL_VC2); // next measured parameter
				_bq76925.calculate_cell_voltage(_data[13], 1);
			//	set_status(_data[13]);
				//calculating vc1 voltage
						break;
			case 2:
				_bq76925.set(_bq76925.CELL_CTL, _bq76925.VCOUT_SEL_VCn  + _bq76925.CELL_SEL_VC3); // next measured parameter
				_bq76925.calculate_cell_voltage(_data[14], 2);
			//	set_status(_data[14]);
				//calculating vc1 voltage
						break;
			case 3:
				_bq76925.set(_bq76925.CELL_CTL,_bq76925.VCOUT_SEL_VCn  + _bq76925.CELL_SEL_VTEMP); // next measured parameter
				_bq76925.calculate_cell_voltage(_data[15], 3);
			//	set_status(_data[15]);
				//calculating vc1 voltage
						break;
			case 4:
				_bq76925.set(_bq76925.CELL_CTL,_bq76925.VCOUT_SEL_5VREF); // next measured parameter
				//calculating bq internal temperature
				_bq76925.calculate_internal_temperature(_data[16]);
				set_status(_data[16]);
				break;
			}
			_begin_voltage_settling_time = HAL_GetTick();
			_state_machine = start;
			if(++_j == 5)
				_j = 0;
		}
		return true;
	}
	return false;

}


void BMS::read_measurements(vector<VALUES> &data){
		data=_data;
}

void BMS::reset_after_error(){

	_status = {1,0,0,0,0,0};
	_state_machine=start;
	_j=0;
	_bq76925.init();
	VALUES_reset();
}



void BMS::set_status(VALUES &element){
	if(element.value > element.threshold)
	{
		*element.error = 1;
		_status.READY = 0;
	}
}
