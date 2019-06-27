#ifdef APP_CRAMB

#include "cabinet.h"
#include "task_cramb_axial.h"
#include "../../sys/scheduler.h"
#include "../../sys/debug.h"
#include "clarke_transform.h"
#include "../../drv/pwm.h"
#include "../../drv/analog.h"

#define Ts	(1.0 / TASK_CRAMB_AXIAL_UPDATES_PER_SEC)// sample time
#define R_DEFAULT (20.0)
#define L_DEFAULT (0.0150)
#define CC_BANDWIDTH_DEFAULT (300) //Hz
#define VDC_DEFAULT (20)//
#define DEFAULT_INVERTER (3)

static int inverter_num = DEFAULT_INVERTER;
static double Vdc = VDC_DEFAULT; //DC bus voltage
static double Ki = R_DEFAULT*CC_BANDWIDTH_DEFAULT*PI2; //integral control gain
static double Kp = L_DEFAULT*CC_BANDWIDTH_DEFAULT*PI2;  //proportional control gain

static task_control_block_t tcb;

//alpha-beta frame current commands (alpha is element 0)
static double I_star = 0.0;

//integrated error for integral control
static double accum_error = 0.0;

//register and begin task
void task_cramb_axial_init(void)
{
	//populate struct
	scheduler_tcb_init(&tcb, task_cramb_axial_callback, NULL, "cramb_axial", TASK_CRAMB_AXIAL_INTERVAL_USEC);

	scheduler_tcb_register(&tcb);
}

//stop task and pwm outputs
void task_cramb_axial_deinit(void)
{
	scheduler_tcb_unregister(&tcb);

	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxA, 0.0);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxB, 0.0);

	//reset commanded currents to zero
	I_star = 0.0;
}
//function to determine if task has been started
uint8_t task_cramb_axial_is_inited(void)
{
	return scheduler_tcb_is_registered(&tcb);
}

//call back function to be run repeatedly
void task_cramb_axial_callback(void *arg)
{
	//read actual three-phase currents
	double I_actual; //three phase currents measured by adc
	read_current_axial(&I_actual); //read three phase currents

	//calculate current errors
	double error; //initiate error variables
	error = I_star - I_actual;

	//initiate commanded voltage
	double v_star;

	//alpha controller
	accum_error += error*Ts;
	v_star = Kp*error + Ki*accum_error;

	//write out line to neutral voltages
	set_voltage_axial(v_star);

}


void read_current_axial(double *I_actual){

	float adc_value;

	analog_getf(cabinet_lookup[inverter_num].adcChA, &adc_value);

	*I_actual = (double) (adc_value*cabinet_lookup[inverter_num].adcGainA + cabinet_lookup[inverter_num].adcOffsetA);

}

void set_current_axial(double I_commanded){

	I_star = I_commanded;

}

void set_voltage_axial(double V){

	//sets line to neutral voltages assuming neutral point is at Vdc/2

	double da, db;

	da = 0.5 + (V/2.0)/(Vdc);
	db = 0.5 - (V/2.0)/(Vdc);

	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxA, da);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxB, db);
}

void set_pole_volts_axial(double a, double b){

	//sets line to neutral voltages assuming neutral point is at Vdc/2

	double da, db;

	da = a/(Vdc);
	db = b/(Vdc);

	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxA, da);
	pwm_set_duty(cabinet_lookup[inverter_num].pwmIdxB, db);
}


void set_vdc_axial(double DC_bus){

	Vdc = DC_bus;

}

void set_cc_gains_axial(double R_in, double L_in, double fb){

	Ki = R_in*fb*PI2;
	Kp = L_in*fb*PI2;

}

void read_adc_axial(double *adc){

	float phase_a_adc;

	analog_getf(cabinet_lookup[inverter_num].adcChA, &phase_a_adc);

	*adc = (double) phase_a_adc;

}

void set_inverter_axial(int inv_num){

	inverter_num = inv_num;

}


#endif //APP_CRAMB
