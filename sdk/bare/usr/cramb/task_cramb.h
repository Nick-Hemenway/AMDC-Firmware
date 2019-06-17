#ifndef TASK_CRAMB_H
#define TASK_CRAMB_H

#include "../../sys/defines.h"

#define TASK_CRAMB_UPDATES_PER_SEC		(1)
#define TASK_CRAMB_INTERVAL_USEC		(USEC_IN_SEC / TASK_CRAMB_UPDATES_PER_SEC)

void task_cramb_init(void);
void task_cramb_deinit(void);
void task_cramb_callback(void *arg);
uint8_t task_cramb_is_inited(void);

#endif // TASK_CRAMB_H
