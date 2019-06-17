#ifdef APP_CRAMB

#include "app_cramb.h"
#include "cmd/cmd_cramb.h"

void app_cramb_init(void)
{
	cmd_cramb_register();
}

#endif //APP_CRAMB
