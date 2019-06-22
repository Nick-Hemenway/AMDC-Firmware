#ifdef APP_BETA_LABS

#include "task_mo.h"
#include "machine.h"
#include "../../sys/scheduler.h"
#include "../../drv/encoder.h"

static task_control_block_t tcb;

static double task_mo_omega_m = 0.0;

// Tunings for LPF on omega_m
#define A_1Hz	(0.9993718788200349)
#define A_5Hz	(0.9968633369849541)
#define A_10Hz	(0.9937365126247782)
#define A_50Hz	(0.9690724263048106)
#define A_100Hz	(0.9391013674242926)
#define A_500Hz	(0.7304026910486456)

#define A (A_100Hz)

static double filter(double input)
{
	static double z1 = 0.0;

	double output = (input * (1 - A)) + z1;

	z1 = output * A;

	return output;
}


void task_mo_init(void)
{
	scheduler_tcb_init(&tcb, task_mo_callback, NULL, "mo", TASK_MO_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_mo_callback(void *arg)
{
	// TODO: change this to use "position"
	// and wrap things etc
	// Also be aware of negative wraps

	static int32_t last_steps = 0;
	int32_t steps;
	encoder_get_steps(&steps);
	int32_t delta = steps - last_steps;
	double rads = PI2 * ((double) delta / (double) (1 << ENCODER_PULSES_PER_REV_BITS));

	last_steps = steps;

	double omega_m = rads * (double) TASK_MO_UPDATES_PER_SEC;

	task_mo_omega_m = filter(omega_m);
}

void task_mo_get_omega_e(double *omega_e)
{
	*omega_e = task_mo_omega_m * POLE_PAIRS;
}

void task_mo_get_omega_m(double *omega_m)
{
	*omega_m = task_mo_omega_m;
}


#endif // APP_BETA_LABS
