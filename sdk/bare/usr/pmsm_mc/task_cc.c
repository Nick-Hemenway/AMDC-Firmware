#ifdef APP_PMSM_MC

#include "task_cc.h"
#include "machine.h"
#include "cmd/cmd_cc.h"
#include "../../sys/debug.h"
#include "../../sys/defines.h"
#include "../../sys/scheduler.h"
#include "../../sys/transform.h"
#include "../../drv/analog.h"
#include "../../drv/encoder.h"
#include "../../drv/io.h"
#include "../../drv/dac.h"
#include "../../drv/pwm.h"
#include <stdio.h>
#include <math.h>

#define Wb					(CC_BANDWIDTH * PI2) // rad/s
#define Ts					(1.0 / TASK_CC_UPDATES_PER_SEC)
#define Kp_d				(Wb * Ld_HAT)
#define Kp_q				(Wb * Lq_HAT)
#define Ki_d				((Rs_HAT / Ld_HAT) * Kp_d)
#define Ki_q				((Rs_HAT / Lq_HAT) * Kp_q)

static double Id_star = 0.0;
static double Iq_star = 0.0;

static int32_t dq_offset = 9550; // 9661 from beta-axis injection

static double Id_err_acc = 0.0;
static double Iq_err_acc = 0.0;

static double theta_da = 0.0;

inline static int saturate(double min, double max, double *value) {
	if (*value < min) {
		// Lower bound saturation
		*value = min;
		return -1;
	} else if (*value > max) {
		// Upper bound saturation
		*value = max;
		return 1;
	} else {
		// No saturation
		return 0;
	}
}

static task_control_block_t tcb;


uint8_t task_cc_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

void task_cc_init(void)
{
	// Register task with scheduler
	scheduler_tcb_init(&tcb, task_cc_callback, NULL, "cc", TASK_CC_INTERVAL_USEC);
	scheduler_tcb_register(&tcb);
}

void task_cc_deinit(void)
{
	scheduler_tcb_unregister(&tcb);
}

void task_cc_set_Id_star(double my_Id_star) { Id_star = my_Id_star; }
void task_cc_set_Iq_star(double my_Iq_star) { Iq_star = my_Iq_star; }
void task_cc_set_dq_offset(int32_t offset) { dq_offset = offset; }

static void _get_theta_da(double *theta_da)
{
	// Get raw encoder position
	uint32_t position;
	encoder_get_position(&position);

	// Add offset (align to DQ frame)
	position += dq_offset;

	while (position >= ENCODER_PULSES_PER_REV) {
		position -= ENCODER_PULSES_PER_REV;
	}

	// Convert to radians
	*theta_da = (double)PI2 * ((double)position / (double)ENCODER_PULSES_PER_REV);

	// Multiple by pole pairs to convert mechanical to electrical degrees
	*theta_da *= POLE_PAIRS;

	// Mod by 2 pi
	while (*theta_da > PI2) {
		*theta_da -= PI2;
	}
}

static void _get_Iabc(double *Iabc)
{
	// Read from ADCs
	float Iabc_f[3];
	analog_getf(CC_PHASE_A_ADC, &Iabc_f[0]);
	analog_getf(CC_PHASE_B_ADC, &Iabc_f[1]);
	analog_getf(CC_PHASE_C_ADC, &Iabc_f[2]);

	// Convert ADC values to raw currents
	Iabc[0] = ((double) Iabc_f[0] * ADC_TO_AMPS_PHASE_A_GAIN) + ADC_TO_AMPS_PHASE_A_OFFSET;
	Iabc[1] = ((double) Iabc_f[1] * ADC_TO_AMPS_PHASE_B_GAIN) + ADC_TO_AMPS_PHASE_B_OFFSET;
	Iabc[2] = ((double) Iabc_f[2] * ADC_TO_AMPS_PHASE_C_GAIN) + ADC_TO_AMPS_PHASE_C_OFFSET;
}

//#define CC_FIND_DQ_FRAME_OFFSET

void task_cc_callback(void *arg)
{
	// -------------------
	// (0) Update theta_da
	// -------------------
#ifndef CC_FIND_DQ_FRAME_OFFSET
	_get_theta_da(&theta_da);
#endif

	// ----------------------
	// (1) Get current values
	// ----------------------
	double Iabc[3];
	_get_Iabc(Iabc);


	// ---------------------
	// (2) Convert ABC to DQ
	// ---------------------
#ifndef CC_FIND_DQ_FRAME_OFFSET
	double Idq0[3];
	transform_dqz(theta_da, Iabc, Idq0);
#else
	double Idq0[3];
	double Ixyz[3];
	transform_clarke(Iabc, Ixyz);
	Ixyz[1] = 1.0;
	transform_park(theta_da, Ixyz, Idq0);

	// Get raw encoder position
	static int counter = 0;
	const static int SAMPLES_PER_SEC = 5;

	counter++;
	if (counter >= TASK_CC_UPDATES_PER_SEC / SAMPLES_PER_SEC) {
		counter = 0;

		uint32_t position;
		encoder_get_position(&position);

		debug_printf("%ld\r\n", position);
	}
#endif

	// -----------------------------
	// (3) Run through block diagram
	//     to get Vdq_star
	// -----------------------------

	double Id = Idq0[0];
	double Iq = Idq0[1];

	// d-axis
	double Id_err;
	double Vd_star;
	Id_err = Id_star - Id;
	Id_err_acc += Id_err;
	Vd_star = (Kp_d * Id_err) + (Ki_d * Ts * Id_err_acc);

	// q-axis
	double Iq_err;
	double Vq_star;
	Iq_err = Iq_star - Iq;
	Iq_err_acc += Iq_err;
	Vq_star = (Kp_q * Iq_err) + (Ki_q * Ts * Iq_err_acc);


#if 0
	dac_set_output(0, Vd_star, -10, 10);
	dac_set_output(1, Vq_star, -10, 10);
#endif

#if 0

	static double Vq_avg = 0.0;
	static double Vd_avg = 0.0;
	Vq_avg += Vq_star;
	Vd_avg += Vd_star;

	static int counter = 0;
	const static int SAMPLES_PER_SEC = 5;

	counter++;
	if (counter >= TASK_CC_UPDATES_PER_SEC / SAMPLES_PER_SEC) {
		Vq_avg /= counter;
		Vd_avg /= counter;

		counter = 0;

		debug_printf("%f\t%f\r\n", Vd_avg, Vq_avg);
	}

#endif


	// --------------------------------
	// (4) Perform inverse DQ transform
	//     of Vdq_star
	// --------------------------------

	double Vabc_star[3];
	double Vdq0[3];
	Vdq0[0] = Vd_star;
	Vdq0[1] = Vq_star;
	Vdq0[2] = 0.0;
	transform_dqz_inverse(theta_da, Vabc_star, Vdq0);

	// ------------------------------------
	// Saturate Vabc_star to CC_BUS_VOLTAGE
	// ------------------------------------
	io_led_color_t color = {0, 0, 0};
	if (saturate(-CC_BUS_VOLTAGE, CC_BUS_VOLTAGE, &Vabc_star[0]) != 0) color.g = 255;
	if (saturate(-CC_BUS_VOLTAGE, CC_BUS_VOLTAGE, &Vabc_star[1]) != 0) color.g = 255;
	if (saturate(-CC_BUS_VOLTAGE, CC_BUS_VOLTAGE, &Vabc_star[2]) != 0) color.g = 255;
	io_led_set_c(0, 1, 0, &color);

	// --------------------------------------
	// (5) Write voltages out to PWM hardware
	// --------------------------------------

	// Vabc = -Vbus => d = 0.0
	// Vabc =    0V => d = 0.5
	// Vabc = +Vbus => d = 1.0

	pwm_set_duty(CC_PHASE_A_PWM_LEG_IDX, (((Vabc_star[0] / CC_BUS_VOLTAGE) + 1.0) / 2.0));
	pwm_set_duty(CC_PHASE_B_PWM_LEG_IDX, (((Vabc_star[1] / CC_BUS_VOLTAGE) + 1.0) / 2.0));
	pwm_set_duty(CC_PHASE_C_PWM_LEG_IDX, (((Vabc_star[2] / CC_BUS_VOLTAGE) + 1.0) / 2.0));
}

#endif // APP_PMSM_MC
