/*
 * bq76925.cpp
 *
 *  Created on: Jan 14, 2024
 *      Author: Jakub
 */

#include "extended_files\bq76925.h"

	BQ76925::BQ76925(I2C_HandleTypeDef *hi2c):
		VREF_CAL_REGISTER_DATA(0),
		VC1_CAL_REGISTER_DATA(0),
		VC2_CAL_REGISTER_DATA(0),
		VC3_CAL_REGISTER_DATA(0),
		VREF_CAL_EXT_REGISTER_DATA(0),
		VC_CAL_EXT_1_REGISTER_DATA(0),
		VC_CAL_EXT_2_REGISTER_DATA(0),
		GCvc1out(0),
		GCvc2out(0),
		GCvc3out(0),
		OCvc1out(0),
		OCvc2out(0),
		OCvc3out(0),
		GCvref(0),
		_hi2c(hi2c){

		// initialisation of bq module
		MX_I2C2_Init();
		init();

	}

	void BQ76925::init(){
		// configuring bq
		while(!set(POWER_CTL, REF_EN+VTB_EN+VC_AMP_EN+I_AMP_EN+I_COMP_EN));
		while(!set(CONFIG_2,REF_SEL));
		// getting registers data for parameter correction
		while(!get(VREF_CAL, VREF_CAL_REGISTER_DATA));
		while(!get(VC1_CAL, VC1_CAL_REGISTER_DATA));
		while(!get(VC2_CAL, VC2_CAL_REGISTER_DATA));
		while(!get(VC3_CAL, VC3_CAL_REGISTER_DATA));
		while(!get(VREF_CAL_EXT, VREF_CAL_EXT_REGISTER_DATA));
		while(!get(VC_CAL_EXT_1, VC_CAL_EXT_1_REGISTER_DATA));
		while(!get(VC_CAL_EXT_2, VC_CAL_EXT_2_REGISTER_DATA));

		while(!set(CELL_CTL,VCOUT_SEL_5VREF)); // first measured parameter

		//create correction values due to the datasheet equations - code is written this way because (i thint) its easier to understand by human - but still not easy
		// VCn = (Vwe * GCref + OCvcout)/Gvcout * (1+GCvcout);
		// GCvcout = [(VCn_GC_4<<4) + VCn_GAIN_CORR] * 0.001
		// OCvcout = [(VCn_OC_4<<4) + VCn_OFFSET_CORR] * 0.001
		// GCref = (1+ [ ( VREF_GC_4 <<4) + VREF_GAIN_CORR] * 0.001) + ([VREF_OC_5<<5) + (VREF_OC_4<<4)+VREF_OFFSET_CORR] * 0.001)/VREFnominal
		GCvc1out = U2_to_int((data_interpretation(VC_CAL_EXT_1_REGISTER_DATA,VC1_GC_4,6) << 4) + data_interpretation(VC1_CAL_REGISTER_DATA,VC1_GAIN_CORR,0),5)*0.001;
		GCvc2out = U2_to_int((data_interpretation(VC_CAL_EXT_1_REGISTER_DATA,VC2_GC_4,4) << 4) + data_interpretation(VC2_CAL_REGISTER_DATA,VC2_GAIN_CORR,0),5)*0.001;
		GCvc3out = U2_to_int((data_interpretation(VC_CAL_EXT_2_REGISTER_DATA,VC3_GC_4,6) << 4) + data_interpretation(VC3_CAL_REGISTER_DATA,VC3_GAIN_CORR,0),5)*0.001;

		OCvc1out = U2_to_int((data_interpretation(VC_CAL_EXT_1_REGISTER_DATA,VC1_OC_4,7) << 4) + data_interpretation(VC1_CAL_REGISTER_DATA,VC1_OFFSET_CORR,4),5)*0.001;
		OCvc2out = U2_to_int((data_interpretation(VC_CAL_EXT_1_REGISTER_DATA,VC2_OC_4,5) << 4) + data_interpretation(VC2_CAL_REGISTER_DATA,VC2_OFFSET_CORR,4),5)*0.001;
		OCvc3out = U2_to_int((data_interpretation(VC_CAL_EXT_2_REGISTER_DATA,VC3_OC_4,7) << 4) + data_interpretation(VC3_CAL_REGISTER_DATA,VC3_OFFSET_CORR,4),5)*0.001;

		_A = (1 + U2_to_int((data_interpretation(VREF_CAL_EXT_REGISTER_DATA,VREF_GC_4,0) << 4) + data_interpretation(VREF_CAL_REGISTER_DATA,VREF_GAIN_CORR,0),0)*0.001);
		_B = (U2_to_int((data_interpretation(VREF_CAL_EXT_REGISTER_DATA,VREF_OC_5,2) << 5) +
				(data_interpretation(VREF_CAL_EXT_REGISTER_DATA,VREF_OC_4,1) << 4) +
				data_interpretation(VREF_CAL_REGISTER_DATA,VREF_OFFSET_CORR,4),6)*0.001);

		recalculate_GCvref();

		HAL_Delay(50);
	}
	void BQ76925::recalculate_GCvref(){
		GCvref = _A + _B/_ref_voltage;
	}


	inline uint8_t BQ76925::data_interpretation(uint8_t &reg,const uint8_t mask, const uint8_t data_position){
		return (reg&mask)>>data_position;

	}
	inline int BQ76925::U2_to_int(uint8_t data, uint8_t bit_size){
		// nie jestem pewnien czy dobrze zaimplementowana jest ta konwersja
		 // Convert U2 to signed integer
		    int signed_value = static_cast<int>(data);
		    if ((signed_value & (1 << (bit_size - 1))) != 0) {
		        // Negative value, apply Two's complement
		    	signed_value -= (1 << bit_size);
		    }
		    return signed_value;
	}

	bool BQ76925::set(uint8_t reg, uint8_t data){
		_tick_start= HAL_GetTick();
		while(HAL_I2C_Master_Transmit(_hi2c,reg<<1, &data, 1, HAL_MAX_DELAY) != HAL_OK){
			if( HAL_GetTick()-_tick_start>1)
				return false;
			}
		return true;
	}

	bool BQ76925::get(uint8_t reg, uint8_t &data){
		_tick_start= HAL_GetTick();
		while( HAL_I2C_Master_Receive(_hi2c,reg<<1, &data, 1, HAL_MAX_DELAY) != HAL_OK){
			if( HAL_GetTick()-_tick_start>1)
				return false;
			}
		return true;

	}

	void BQ76925::calculate_internal_temperature(VALUES & element){
		element.value = (element.value - V25 )/dV + 25.0 ;
	}
	void BQ76925::calculate_internal_reference_voltage(VALUES & element){
		_ref_voltage = element.value *= 2;
		recalculate_GCvref();
	}
	void BQ76925::calculate_low_current(VALUES & element){

	}
	void BQ76925::calculate_cell_voltage(VALUES & element, const uint8_t cell){
		if(cell == 1){
			element.value =  (element.value * GCvref + OCvc1out)/Gvcout * (1+GCvc1out);
		}
		else if(cell == 2){
			element.value =  (element.value * GCvref + OCvc2out)/Gvcout * (1+GCvc2out);
		}
		else if(cell == 3){
			element.value =  (element.value * GCvref + OCvc3out)/Gvcout * (1+GCvc3out);
		}
	}



