#ifndef TASKMATE_PRIVATE_H
#define TASKMATE_PRIVATE_H

#include "sysCore/TaskMate_define.h"

Task task_table[TASK_COUNT];
int8_t task_current = 0;


void taskCreate(void (*taskFunction)(void), uint8_t task_id);

#endif
