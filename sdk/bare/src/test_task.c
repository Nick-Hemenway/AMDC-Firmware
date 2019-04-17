#include "test_task.h"
#include "scheduler.h"
#include "sinetable.h"

#include "../bsp/bsp.h"

#include <stdio.h>
#include <math.h>

#define TEST_VOLTAGE_FREQ	(60) // Hz

#define PI 3.14159265
#define PI2 6.283185307179586
#define PI23 2.094395102393195

double Vpercent = 75.0 / 100.0;
double theta = 0;

void test_task_init(void)
{
	printf("Initializing test task...\n");
	scheduler_register_task(test_task_callback, TEST_TASK_INTERVAL_USEC);
}

void test_task_callback(void)
{
	uint8_t duty1, duty2, duty3;
	double update_da = (2*PI*TEST_VOLTAGE_FREQ) / (double)TEST_TASK_UPDATES_PER_SEC;

	theta += update_da;
	if (theta > PI2)
		theta -= PI2;

	double percent1 = Vpercent*cos(theta);
	double percent2 = Vpercent*cos(theta - PI23);
	double percent3 = Vpercent*cos(theta + PI23);

	duty1 = (uint8_t) 127*(1 + percent1);
	duty2 = (uint8_t) 127*(1 + percent2);
	duty3 = (uint8_t) 127*(1 + percent3);

	pwm_set_duty_raw(0, duty1);
	pwm_set_duty_raw(1, duty2);
	pwm_set_duty_raw(2, duty3);

//
//	pwm_set_duty(0, 0.1);
//	pwm_set_duty(1, 0.1);
//	pwm_set_duty(2, 0.1);
//	printf("%f\n", t);

	//	double out = (cos(2 * M_PI * TEST_VOLTAGE_FREQ * t) + 1) / 2.0;
//	t += dt;


//	pwm_set_duty(23, out);
//	pwm_set_duty(0, sinetable[idx]);
//	pwm_set_duty(1, sinetable[idx]);
//	pwm_set_duty(2, sinetable[idx]);
//
//	idx++;
//	if (idx >= SINETABLE_LENGTH) {
//		idx = 0;
//	}
}
