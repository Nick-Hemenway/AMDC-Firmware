#ifdef APP_CRAMB

#include "cabinet.h"
#include "task_cramb.h"
#include "../../sys/scheduler.h"
#include "../../sys/debug.h"
#include "machine.h"
#include "clarke_transform.h"
#include "../../drv/pwm.h"
#include "../../drv/analog.h"

static task_control_block_t tcb;

//current control parameters:
#define Wb					(CC_BANDWIDTH * PI2) // rad/s
#define Ts					(1.0 / TASK_CRAMB_UPDATES_PER_SEC)
#define Ki  				(R*Wb)
#define Kp	    			(L*Wb)

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

	pwm_set_duty(CAB_INV6_PHA, 0.0);
	pwm_set_duty(CAB_INV6_PHB, 0.0);
	pwm_set_duty(CAB_INV6_PHC, 0.0);

	//reset commanded currents to zero
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

	analog_getf(CAB_INV6_PHA_ADC, &phase_a_adc);
	analog_getf(CAB_INV6_PHB_ADC, &phase_b_adc);
	analog_getf(CAB_INV6_PHC_ADC, &phase_c_adc);

	abc[0] = phase_a_adc*CAB_INV6_PHA_ADC_GAIN + CAB_INV6_PHA_ADC_OFFSET;
	abc[1] = phase_b_adc*CAB_INV6_PHB_ADC_GAIN + CAB_INV6_PHB_ADC_OFFSET;
	abc[2] = phase_c_adc*CAB_INV6_PHC_ADC_GAIN + CAB_INV6_PHC_ADC_OFFSET;

}

void set_currents(double I1, double I2){

	double abc_star[] = {I1, I2, -I1 - I2}; //input

	//write alpha beta currents to global variable
	forward_clarke(abc_star, alpha_beta_star);

}

void set_ln_voltages(double a, double b, double c){

	//sets line to neutral voltages assuming neutral point is at Vdc/2

	double da, db, dc;

	da = 0.5 + a/(DC_BUS);
	db = 0.5 + b/(DC_BUS);
	dc = 0.5 + c/(DC_BUS);

	pwm_set_duty(CAB_INV6_PHA, da);
	pwm_set_duty(CAB_INV6_PHB, db);
	pwm_set_duty(CAB_INV6_PHC, dc);
}

void set_pole_voltages(double A, double B, double C){

	//sets each of the three pole voltages manually
	double da, db, dc;

	da = A/DC_BUS;
	db = B/DC_BUS;
	dc = C/DC_BUS;

	pwm_set_duty(CAB_INV6_PHA, da);
	pwm_set_duty(CAB_INV6_PHB, db);
	pwm_set_duty(CAB_INV6_PHC, dc);
}


#endif //APP_CRAMB
