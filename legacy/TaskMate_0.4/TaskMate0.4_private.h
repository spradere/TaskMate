#ifndef TASKMATE0_4_PRIVATE_H
#define TASKMATE0_4_PRIVATE_H

#include "TaskMate0.4_define.h"

Task task_table[TASK_COUNT];
int8_t task_current = 0;


void taskCreate(void (*taskFunction)(void), uint8_t task_id);

#endif
