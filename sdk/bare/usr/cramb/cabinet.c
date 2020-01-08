#include "../../drv/analog.h"
#include "cabinet.h"

//PWM CHANNELS
#define CAB_INV1_PHA	(1)
#define CAB_INV1_PHB	(0)

#define CAB_INV2_PHA	(4)
#define CAB_INV2_PHB	(3)

#define CAB_INV3_PHA	(7)
#define CAB_INV3_PHB	(6)

#define CAB_INV4_PHA	(10)
#define CAB_INV4_PHB	(9)

#define CAB_INV5_PHA	(16)
#define CAB_INV5_PHB	(17)
#define CAB_INV5_PHC	(15)

#define CAB_INV6_PHA	(22)
#define CAB_INV6_PHB	(23)
#define CAB_INV6_PHC	(21)

//Single phase inverter current sensor channels
#define CAB_INV1_ADC (ANLG_CHNL1)
#define CAB_INV2_ADC (ANLG_CHNL2)
#define CAB_INV3_ADC (ANLG_CHNL3)
#define CAB_INV4_ADC (ANLG_CHNL4)

//Three-phase inverter current sensor channels
#define CAB_INV5_PHA_ADC (ANLG_CHNL5)
#define CAB_INV5_PHB_ADC (ANLG_CHNL6)
#define CAB_INV5_PHC_ADC (ANLG_CHNL7)

#define CAB_INV6_PHA_ADC (ANLG_CHNL10)
#define CAB_INV6_PHB_ADC (ANLG_CHNL11)
#define CAB_INV6_PHC_ADC (ANLG_CHNL9)

//ADC current sensor gains
//NOTE: Only inverters  with "//calibrated" comment have actually been calibrated
#define CAB_INV1_ADC_GAIN (-0.510765)
#define CAB_INV2_ADC_GAIN (-0.510765)
#define CAB_INV3_ADC_GAIN (-0.5000394) //calibrated
#define CAB_INV4_ADC_GAIN (-0.5035198) //calibrated

#define CAB_INV5_PHA_ADC_GAIN (-0.510765)
#define CAB_INV5_PHB_ADC_GAIN (-0.510356)
#define CAB_INV5_PHC_ADC_GAIN (-0.51092)

#define CAB_INV6_PHA_ADC_GAIN (-0.510765) //calibrated
#define CAB_INV6_PHB_ADC_GAIN (-0.510356) //calibrated
#define CAB_INV6_PHC_ADC_GAIN (-0.51092)  //calibrated

//ADC current sensor offsets
#define CAB_INV1_ADC_OFFSET (-0.0222095)
#define CAB_INV2_ADC_OFFSET (-0.0222095)
#define CAB_INV3_ADC_OFFSET (0.004503091) //calibrated
#define CAB_INV4_ADC_OFFSET (-0.00001157) //calibrated

#define CAB_INV5_PHA_ADC_OFFSET (-0.0222095)
#define CAB_INV5_PHB_ADC_OFFSET (-0.00136184)
#define CAB_INV5_PHC_ADC_OFFSET (-0.00649711)

#define CAB_INV6_PHA_ADC_OFFSET (-0.0222095)  //calibrated
#define CAB_INV6_PHB_ADC_OFFSET (-0.00136184) //calibrated
#define CAB_INV6_PHC_ADC_OFFSET (-0.00649711) //calibrated


const channel_info_t cabinet_lookup[CABINET_NUM_INVERTERS] = {
	{.pwmIdxA = CAB_INV1_PHA, .pwmIdxB = CAB_INV1_PHB, .pwmIdxC = CAB_INV1_PHB,
	 .adcChA = CAB_INV1_ADC, .adcChB = CAB_INV1_ADC, .adcChC = CAB_INV1_ADC,
	 .adcGainA = CAB_INV1_ADC_GAIN, .adcGainB = CAB_INV1_ADC_GAIN, .adcGainC = CAB_INV1_ADC_GAIN,
	 .adcOffsetA = CAB_INV1_ADC_OFFSET, .adcOffsetB = CAB_INV1_ADC_OFFSET, .adcOffsetC = CAB_INV1_ADC_OFFSET},

	{.pwmIdxA = CAB_INV2_PHA, .pwmIdxB = CAB_INV2_PHB, .pwmIdxC = CAB_INV2_PHB,
	 .adcChA = CAB_INV2_ADC, .adcChB = CAB_INV2_ADC, .adcChC = CAB_INV2_ADC,
	 .adcGainA = CAB_INV2_ADC_GAIN, .adcGainB = CAB_INV2_ADC_GAIN, .adcGainC = CAB_INV2_ADC_GAIN,
	 .adcOffsetA = CAB_INV2_ADC_OFFSET, .adcOffsetB = CAB_INV2_ADC_OFFSET, .adcOffsetC = CAB_INV2_ADC_OFFSET},

	{.pwmIdxA = CAB_INV3_PHA, .pwmIdxB = CAB_INV3_PHB, .pwmIdxC = CAB_INV3_PHB,
	 .adcChA = CAB_INV3_ADC, .adcChB = CAB_INV3_ADC, .adcChC = CAB_INV3_ADC,
	 .adcGainA = CAB_INV3_ADC_GAIN, .adcGainB = CAB_INV3_ADC_GAIN, .adcGainC = CAB_INV3_ADC_GAIN,
	 .adcOffsetA = CAB_INV3_ADC_OFFSET, .adcOffsetB = CAB_INV3_ADC_OFFSET, .adcOffsetC = CAB_INV3_ADC_OFFSET},

	{.pwmIdxA = CAB_INV4_PHA, .pwmIdxB = CAB_INV4_PHB, .pwmIdxC = CAB_INV4_PHB,
	 .adcChA = CAB_INV4_ADC, .adcChB = CAB_INV4_ADC, .adcChC = CAB_INV4_ADC,
 	 .adcGainA = CAB_INV4_ADC_GAIN, .adcGainB = CAB_INV4_ADC_GAIN, .adcGainC = CAB_INV4_ADC_GAIN,
 	 .adcOffsetA = CAB_INV4_ADC_OFFSET, .adcOffsetB = CAB_INV4_ADC_OFFSET, .adcOffsetC = CAB_INV4_ADC_OFFSET},

	{.pwmIdxA = CAB_INV5_PHA, .pwmIdxB = CAB_INV5_PHB, .pwmIdxC = CAB_INV5_PHC,
	 .adcChA = CAB_INV5_PHA_ADC, .adcChB = CAB_INV5_PHB_ADC, .adcChC = CAB_INV5_PHC_ADC,
	 .adcGainA = CAB_INV5_PHA_ADC_GAIN, .adcGainB = CAB_INV5_PHB_ADC_GAIN, .adcGainC = CAB_INV5_PHC_ADC_GAIN,
	 .adcOffsetA = CAB_INV5_PHA_ADC_OFFSET, .adcOffsetB = CAB_INV5_PHB_ADC_OFFSET, .adcOffsetC = CAB_INV5_PHC_ADC_OFFSET},

	{.pwmIdxA = CAB_INV6_PHA, .pwmIdxB = CAB_INV6_PHB, .pwmIdxC = CAB_INV6_PHC,
	 .adcChA = CAB_INV6_PHA_ADC, .adcChB = CAB_INV6_PHB_ADC, .adcChC = CAB_INV6_PHC_ADC,
	 .adcGainA = CAB_INV6_PHA_ADC_GAIN, .adcGainB = CAB_INV6_PHB_ADC_GAIN, .adcGainC = CAB_INV6_PHC_ADC_GAIN,
	 .adcOffsetA = CAB_INV6_PHA_ADC_OFFSET, .adcOffsetB = CAB_INV6_PHB_ADC_OFFSET, .adcOffsetC = CAB_INV6_PHC_ADC_OFFSET}
};
