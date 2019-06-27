#ifndef TASK_CRAMB_AXIAL_H
#define TASK_CRAMB_AXIAL_H

#include "../../sys/defines.h"

#define TASK_CRAMB_AXIAL_UPDATES_PER_SEC		(10000)
#define TASK_CRAMB_AXIAL_INTERVAL_USEC		(USEC_IN_SEC / TASK_CRAMB_AXIAL_UPDATES_PER_SEC)

void task_cramb_axial_init(void);
void task_cramb_axial_deinit(void);
uint8_t task_cramb_axial_is_inited(void);
void task_cramb_axial_callback(void *arg);
void read_current_axial(double *I_actual);
void set_current_axial(double I);
void set_voltage_axial(double V);
void set_pole_volts_axial(double a, double b);
void set_vdc_axial(double DC_bus);
void set_cc_gains_axial(double R_in, double L_in, double fb);
void read_adc_axial(double *adc);
void set_inverter_axial(int inv_num);


#endif // TASK_CRAMB_H
