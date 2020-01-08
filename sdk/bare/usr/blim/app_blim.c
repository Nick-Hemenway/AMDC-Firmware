#ifdef APP_BLIM

#include "app_blim.h"
#include "cmd/cmd_vsi.h"

void app_blim_init(void)
{
	cmd_vsi_register();
	test();
}

#endif //APP_BLIM
