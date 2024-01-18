/*
 * bms_typedefs.h
 *
 *  Created on: Jan 16, 2024
 *      Author: Jakub
 */

#ifndef INC_EXTENDED_FILES_BMS_TYPEDEFS_H_
#define INC_EXTENDED_FILES_BMS_TYPEDEFS_H_



typedef struct{
	double value;
	char name[100];
	double threshold;
	uint8_t *error;
	uint16_t number_of_samples;
}VALUES;


typedef struct{

	uint8_t READY{1};
	uint8_t AVERAGING_DONE{0};
	uint8_t OVERTEMPERATURE_ERROR{0};
	uint8_t UNDERVOLTAGE_ERROR{0};
	uint8_t OVERCURRENT_ERROR{0};
	uint8_t CHARGE{0};

}STATUS;

enum STATE_MACHINE{start,aquisition,converting};



#endif /* INC_EXTENDED_FILES_BMS_TYPEDEFS_H_ */
