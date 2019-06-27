#ifdef APP_CRAMB

#include "app_cramb.h"
#include "cmd/cmd_cramb.h"
#include "cmd/cmd_cramb_axial.h"

void app_cramb_init(void)
{
	cmd_cramb_register();
	cmd_cramb_axial_register();
}

#endif //APP_CRAMB
