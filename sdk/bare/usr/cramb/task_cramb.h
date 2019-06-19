#ifndef TASK_CRAMB_H
#define TASK_CRAMB_H

#include "../../sys/defines.h"

#define TASK_CRAMB_UPDATES_PER_SEC		(1)
#define TASK_CRAMB_INTERVAL_USEC		(USEC_IN_SEC / TASK_CRAMB_UPDATES_PER_SEC)
#define DC_BUS (10.0)
#define CC_BANDWIDTH (100)

void task_cramb_init(void);
void task_cramb_deinit(void);
void task_cramb_callback(void *arg);
uint8_t task_cramb_is_inited(void);
void read_currents(double *abc);
void set_currents(double I1, double I2);
void set_ln_voltages(double a, double b, double c);
void set_pole_voltages(double A, double B, double C);

#endif // TASK_CRAMB_H
