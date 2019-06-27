#ifdef APP_CRAMB

#include "cabinet.h"
#include "task_cramb.h"
#include "../../sys/scheduler.h"
#include "../../sys/debug.h"
#include "clarke_transform.h"
#include "../../drv/pwm.h"
#include "../../drv/analog.h"

#define Ts	(1.0 / TASK_CRAMB_UPDATES_PER_SEC)// sample time
#define R_DEFAULT (10.0)
#define L_DEFAULT (0.0075)
#define CC_BANDWIDTH_DEFAULT (300) //Hz
#define VDC_DEFAULT (20)//
#define DEFAULT_INVERTER (5)

static int inverter_num = DEFAULT_INVERTER;
static double Vdc = VDC_DEFAULT; //DC bus voltage
static double Ki = R_DEFAULT*CC_BANDWIDTH_DEFAULT*PI2; //integral control gain
static double Kp = L_DEFAULT*CC_BANDWIDTH_DEFAULT*PI2;  //proportional control gain

static task_control_block_t tcb;

//alpha-beta frame current commands (alpha is element 0)
static double alpha_beta_star[] = {0,0};

//integrated error for integral control
static double accum_error_alpha = 0.0;
static double accum_error_beta = 0.0;

//register and begin task
void task_cramb_init(void)
{
	//populate struct
	scheduler_tcb_init(&tcb, task_cramb_callback, NULL, "cramb", TASK_CRAMB_INTERVAL_USEC);

	scheduler_tcb_register(&tcb);
}

//stop task and pwm outputs
void task_cramb_deinit(void)
{
	scheduler_tcb_unregister(&tcb);

	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxA, 0.0);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxB, 0.0);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxC, 0.0);

	//reset commanded currents to ze
	alpha_beta_star[0] = 0.0;
	alpha_beta_star[1] = 0.0;
}
//function to determine if task has been started
uint8_t task_cramb_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

//call back function to be run repeatedly
void task_cramb_callback(void *arg)
{
	//read actual three-phase currents
	double abc_actual[3]; //three phase currents measured by adc
	read_currents(abc_actual); //read three phase currents

	//convert measured currents to alpha-beta frame
	double alpha_beta_actual[2];
	forward_clarke(abc_actual, alpha_beta_actual); //measured alpha-beta currents

	//calculate current errors
	double error_alpha, error_beta; //initiate error variables
	error_alpha = alpha_beta_star[0] - alpha_beta_actual[0];
	error_beta =  alpha_beta_star[1] - alpha_beta_actual[1];

	//initiate commanded voltages
	double v_alpha_beta_star[2];

	//alpha controller
	accum_error_alpha += error_alpha*Ts;
	v_alpha_beta_star[0] = Kp*error_alpha + Ki*accum_error_alpha;

	//beta controller
	accum_error_beta += error_beta*Ts;
	v_alpha_beta_star[1] = Kp*error_beta + Ki*accum_error_beta;

	//convert voltages back to abc frame
	double v_abc_star[3];
	inverse_clarke(v_alpha_beta_star, v_abc_star);

	//write out line to neutral voltages
	set_ln_voltages(v_abc_star[0], v_abc_star[1], v_abc_star[2]);

}


void read_currents(double *abc){

	float phase_a_adc, phase_b_adc, phase_c_adc;

	analog_getf(cabinet_lookup[inverter_num].adcChA, &phase_a_adc);
	analog_getf(cabinet_lookup[inverter_num].adcChB, &phase_b_adc);
	analog_getf(cabinet_lookup[inverter_num].adcChC, &phase_c_adc);

	abc[0] = phase_a_adc*cabinet_lookup[inverter_num].adcGainA + cabinet_lookup[inverter_num].adcOffsetA;
	abc[1] = phase_b_adc*cabinet_lookup[inverter_num].adcGainB + cabinet_lookup[inverter_num].adcOffsetB;
	abc[2] = phase_c_adc*cabinet_lookup[inverter_num].adcGainC + cabinet_lookup[inverter_num].adcOffsetC;

}

void set_vdc(double DC_bus){

	Vdc = DC_bus;

}

void set_cc_gains(double R_in, double L_in, double fb){

	Ki = R_in*fb*PI2;
	Kp = L_in*fb*PI2;

}

void set_currents(double I1, double I2){

	double abc_star[] = {I1, I2, -I1 - I2}; //input

	//write alpha beta currents to global variable
	forward_clarke(abc_star, alpha_beta_star);

}

void set_ln_voltages(double a, double b, double c){

	//sets line to neutral voltages assuming neutral point is at Vdc/2

	double da, db, dc;

	da = 0.5 + a/(Vdc);
	db = 0.5 + b/(Vdc);
	dc = 0.5 + c/(Vdc);

	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxA, da);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxB, db);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxC, dc);
}

void set_pole_voltages(double A, double B, double C){

	//sets each of the three pole voltages manually
	double da, db, dc;

	da = A/Vdc;
	db = B/Vdc;
	dc = C/Vdc;

	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxA, da);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxB, db);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxC, dc);
}

void set_inverter(int inv_num){

	inverter_num = inv_num;
}

void read_adc(double *abc){

	float phase_a_adc, phase_b_adc, phase_c_adc;

	analog_getf(cabinet_lookup[inverter_num].adcChA, &phase_a_adc);
	analog_getf(cabinet_lookup[inverter_num].adcChB, &phase_b_adc);
	analog_getf(cabinet_lookup[inverter_num].adcChC, &phase_c_adc);

	abc[0] = (double) phase_a_adc;
	abc[1] = (double) phase_b_adc;
	abc[2] = (double) phase_c_adc;

}


#endif //APP_CRAMB
