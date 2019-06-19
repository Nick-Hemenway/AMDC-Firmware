#ifndef CABINET_H
#define CABINET_H

#include "../../drv/analog.h"

//PWM CHANNELS
#define CAB_INV1_PHA	(0)
#define CAB_INV1_PHB	(1)

#define CAB_INV2_PHA	(3)
#define CAB_INV2_PHB	(4)

#define CAB_INV3_PHA	(6)
#define CAB_INV3_PHB	(7)

#define CAB_INV4_PHA	(9)
#define CAB_INV4_PHB	(10)

#define CAB_INV5_PHA	(18)
#define CAB_INV5_PHB	(19)
#define CAB_INV5_PHC	(20)

#define CAB_INV6_PHA	(16)
#define CAB_INV6_PHB	(17)
#define CAB_INV6_PHC	(15)

//Single phase inverter current sensor channels
#define CAB_INV1_ADC (ANLG_CHNL1)
#define CAB_INV2_ADC (ANLG_CHNL2)
#define CAB_INV3_ADC (ANLG_CHNL3)
#define CAB_INV4_ADC (ANLG_CHNL4)

//Three-phase inverter current sensor channels
#define CAB_INV5_PHA_ADC (ANLG_CHNL5)
#define CAB_INV5_PHB_ADC (ANLG_CHNL6)
#define CAB_INV5_PHC_ADC (ANLG_CHNL7)

#define CAB_INV6_PHA_ADC (ANLG_CHNL11)
#define CAB_INV6_PHB_ADC (ANLG_CHNL10)
#define CAB_INV6_PHC_ADC (ANLG_CHNL9)

//ADC current sensor gains
#define CAB_INV6_PHA_ADC_GAIN (-0.4837)
#define CAB_INV6_PHB_ADC_GAIN (-0.4837)
#define CAB_INV6_PHC_ADC_GAIN (-0.4837)

//ADC current sensor offsets
#define CAB_INV6_PHA_ADC_OFFSET (0.0)
#define CAB_INV6_PHB_ADC_OFFSET (0.0)
#define CAB_INV6_PHC_ADC_OFFSET (0.0)

#endif // CABINET_H
