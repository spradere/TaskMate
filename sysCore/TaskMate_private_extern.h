/**
 * @file TaskMate_private_extern.h
 * @brief extrenal header to cut main sys core in multiple files.
 * 
 */
 
#ifndef TASKMATE_EXTERN
#define TASKMATE_EXTERN

#include "sysCore/TaskMate_define.h"

extern driver_table_t driver_table[];

extern task_table_t task_table[];
extern uint8_t task_current;

#endif
