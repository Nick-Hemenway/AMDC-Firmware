#ifndef CABINET_H
#define CABINET_H

#include "../../drv/analog.h"

//Inverter channel info
typedef struct channel_info_t {
	int pwmIdxA;
	int pwmIdxB;
	int pwmIdxC;
	analog_channel_e adcChA;
	analog_channel_e adcChB;
	analog_channel_e adcChC;
	double adcGainA;
	double adcGainB;
	double adcGainC;
	double adcOffsetA;
	double adcOffsetB;
	double adcOffsetC;
} channel_info_t;

#define CABINET_NUM_INVERTERS (6)
extern const channel_info_t cabinet_lookup[CABINET_NUM_INVERTERS];


#endif // CABINET_H
