#include "user_apps.h"

// ENABLED APPLICATIONS:
//
// Uncomment the following defines to compile
// each application into the executable
//
//#define APP_PMSM_MC
//#define APP_DAC_TEST
#define APP_DEADTIME_COMP

#ifdef APP_PMSM_MC
#include "pmsm_mc/app_pmsm_mc.h"
#endif

#ifdef APP_DAC_TEST
#include "dac_test/app_dac_test.h"
#endif

#ifdef APP_DEADTIME_COMP
#include "deadtime_comp/app_deadtime_comp.h"
#endif

void user_apps_init(void)
{
#ifdef APP_PMSM_MC
	app_pmsm_mc_init();
#endif

#ifdef APP_DAC_TEST
	app_dac_test_init();
#endif

#ifdef APP_DEADTIME_COMP
	app_deadtime_comp_init();
#endif
}
