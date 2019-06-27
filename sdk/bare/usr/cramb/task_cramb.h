#ifndef TASK_CRAMB_H
#define TASK_CRAMB_H

#include "../../sys/defines.h"

#define TASK_CRAMB_UPDATES_PER_SEC		(10000)
#define TASK_CRAMB_INTERVAL_USEC		(USEC_IN_SEC / TASK_CRAMB_UPDATES_PER_SEC)

void task_cramb_init(void);
void task_cramb_deinit(void);
void task_cramb_callback(void *arg);
uint8_t task_cramb_is_inited(void);
void read_currents(double *abc);
void set_currents(double I1, double I2);
void set_ln_voltages(double a, double b, double c);
void set_pole_voltages(double A, double B, double C);
void set_vdc(double DC_bus);
void set_cc_gains(double R_in, double L_in, double fb);
void set_inverter(int inv_num);
void read_adc(double *abc);


#endif // TASK_CRAMB_H
