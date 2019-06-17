#ifdef APP_CRAMB

#include "task_cramb.h"
#include "../../sys/scheduler.h"
#include "../../sys/debug.h"

static task_control_block_t tcb;

void task_cramb_init(void)
{
	//populate struct
	scheduler_tcb_init(&tcb, task_cramb_callback, NULL, "cramb", TASK_CRAMB_INTERVAL_USEC);

	scheduler_tcb_register(&tcb);
}

void task_cramb_deinit(void)
{
	scheduler_tcb_unregister(&tcb);
}

uint8_t task_cramb_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_cramb_callback(void *arg)
{
 debug_printf("hello\n\r");
}

#endif //APP_CRAMB
