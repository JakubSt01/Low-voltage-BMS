/*
 * bq76925.h
 *
 *  Created on: Jan 13, 2024
 *      Author: Jakub
 */

#ifndef INC_BQ76925_H_
#define INC_BQ76925_H_

#include "main.h"
#include "i2c.h"
#include "bms_typedefs.h"


class BQ76925{
public:
	//-------------------------------------------------------------------// combined addresses of bq registers
	static const uint8_t I2C_GROUP_ADDR = 0x4;
	static const uint8_t STATUS   		= (I2C_GROUP_ADDR << 3) + 0x00 ; // Read & Write
	static const uint8_t CELL_CTL	 	= (I2C_GROUP_ADDR << 3) + 0x01 ; // Read & Write
	static const uint8_t BAL_CTL 	 	= (I2C_GROUP_ADDR << 3) + 0x02 ; // Read & Write
	static const uint8_t CONFIG_1  	 	= (I2C_GROUP_ADDR << 3) + 0x03 ; // Read & Write
	static const uint8_t CONFIG_2    	= (I2C_GROUP_ADDR << 3) + 0x04 ; // Read & Write
	static const uint8_t POWER_CTL   	= (I2C_GROUP_ADDR << 3) + 0x05 ; // Read & Write
	static const uint8_t CHIP_ID   		= (I2C_GROUP_ADDR << 3) + 0x07 ; // Read Only
	static const uint8_t VREF_CAL 	 	= (I2C_GROUP_ADDR << 3) + 0x10 ; // Read Only
	static const uint8_t VC1_CAL   		= (I2C_GROUP_ADDR << 3) + 0x11 ; // Read Only
	static const uint8_t VC2_CAL   		= (I2C_GROUP_ADDR << 3) + 0x12 ; // Read Only
	static const uint8_t VC3_CAL   		= (I2C_GROUP_ADDR << 3) + 0x13 ; // Read Only
	static const uint8_t VC4_CAL  	 	= (I2C_GROUP_ADDR << 3) + 0x14 ; // Read Only
	static const uint8_t VC5_CAL  	 	= (I2C_GROUP_ADDR << 3) + 0x15 ; // Read Only
	static const uint8_t VC6_CAL  		= (I2C_GROUP_ADDR << 3) + 0x16 ; // Read Only
	static const uint8_t VC_CAL_EXT_1   = (I2C_GROUP_ADDR << 3) + 0x17 ; // Read Only
	static const uint8_t VC_CAL_EXT_2   = (I2C_GROUP_ADDR << 3) + 0x18 ; // Read Only
	static const uint8_t VREF_CAL_EXT   = (I2C_GROUP_ADDR << 3) + 0x1B ; // Read Only

	//-------------------------------------------------------------------// bits in registers
	static const uint8_t BAL_1          = 0x1;
	static const uint8_t BAL_2          = 0x2;
	static const uint8_t BAL_3          = 0x4;

	static const uint8_t VCOUT_SEL_VSS  = 0x00;
	static const uint8_t VCOUT_SEL_VCn  = 0x10;
	static const uint8_t VCOUT_SEL_5VREF= 0x20; // 0.5 vref
	static const uint8_t VCOUT_SEL_8VREF= 0x30; //0.85 vref

	static const uint8_t CELL_SEL_VC1   = 0x0;
	static const uint8_t CELL_SEL_VC2   = 0x1;
	static const uint8_t CELL_SEL_VC3   = 0x2;
	static const uint8_t CELL_SEL_VTEMP = 0x6;

	static const uint8_t REF_EN  		= 0x0;
	static const uint8_t VTB_EN   		= 0x1;
	static const uint8_t VC_AMP_EN   	= 0x2;
	static const uint8_t I_AMP_EN 		= 0x4;
	static const uint8_t I_COMP_EN 		= 0x8;

	//config 2
	static const uint8_t REF_SEL  		= 0x1;

	// variables containing offset and gain corrections with data masks

	//VREF_CAL address
	uint8_t VREF_CAL_REGISTER_DATA;
	static const uint8_t VREF_OFFSET_CORR = 0xf0;
	static const uint8_t VREF_GAIN_CORR = 0x0f;

	//VCn_CAL address
	uint8_t VC1_CAL_REGISTER_DATA;
	static const uint8_t VC1_OFFSET_CORR = 0xf0;
	static const uint8_t VC1_GAIN_CORR = 0x0f;

	uint8_t VC2_CAL_REGISTER_DATA;
	static const uint8_t VC2_OFFSET_CORR = 0xf0;
	static const uint8_t VC2_GAIN_CORR = 0x0f;

	uint8_t VC3_CAL_REGISTER_DATA;
	static const uint8_t VC3_OFFSET_CORR = 0xf0;
	static const uint8_t VC3_GAIN_CORR = 0x0f;

	//VREF_CAL_EXT address
	uint8_t VREF_CAL_EXT_REGISTER_DATA;
	static const uint8_t VREF_OC_5 = 0x04;
	static const uint8_t VREF_OC_4 = 0x02;
	static const uint8_t VREF_GC_4 = 0x01;


	//VC_CAL_EXT_1 address
	uint8_t VC_CAL_EXT_1_REGISTER_DATA;
	static const uint8_t VC1_OC_4 = 0x80;
	static const uint8_t VC1_GC_4 = 0x40;
	static const uint8_t VC2_OC_4 = 0x20;
	static const uint8_t VC2_GC_4 = 0x10;

	//VC_CAL_EXT_2 address
	uint8_t VC_CAL_EXT_2_REGISTER_DATA;
	static const uint8_t VC3_OC_4 = 0x80;
	static const uint8_t VC3_GC_4 = 0x40;


	// variables used later in equations (all else are used only in initialisation)
	// assuming that reference voltage is 3V - gain correction is given by the datasheet
	static constexpr double Gvcout = 0.6;

	double GCvc1out;
	double GCvc2out;
	double GCvc3out;

	double OCvc1out;
	double OCvc2out;
	double OCvc3out;

	double GCvref;


	// basic commands to use with i2c
	BQ76925(I2C_HandleTypeDef *hi2c);

	void init();
	bool set(uint8_t reg, uint8_t data);
	bool get(uint8_t reg, uint8_t &data);
	 /*

	        We need to interpret data : Example Bitwise AND and Right-Shift Calculation:

	           00101100   (Data)
	         & 00110000   (Mask)
	        __________
	           00100000   (Result)

	        3. Right-Shifting by 4 bits:
	           Shifted Result: 0x00000010 (0b0000 0010)

	           data_position is like "bits needed to be cut from begin "
	    */

	inline uint8_t data_interpretation(uint8_t &reg, const uint8_t mask, const uint8_t data_position);
	// data are stored as 5bit integers - conversion is needed into int
	inline int U2_to_int(uint8_t data, uint8_t bit_size);

	void calculate_internal_temperature(VALUES & element);
	void calculate_internal_reference_voltage(VALUES & element);
	void calculate_low_current(VALUES & element);
	void calculate_cell_voltage(VALUES & element, const uint8_t cell);

	// updates every Vref measurement
	double _A;
	double _B;
	void recalculate_GCvref();

private:
	//i2c handlers
	I2C_HandleTypeDef *_hi2c;

	// dV values and V25 for internal temperature measurement
	const double V25 = 1.2;
	const double dV = -0.0044;

	double _ref_voltage; // changed by calculate_internal_reference_voltage method

	uint32_t _tick_start;

};

#endif /* INC_BQ76925_H_ */
