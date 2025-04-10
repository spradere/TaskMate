/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file listCmd.c
 * @brief command parser for list file options
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/listCmd.h"
// get TaskMate flag bits
#include "src/sysCore/status_bits.h"


void cmdTaskUser(typeof(list_table->task_list[0]->status) *status){*status &= ~(1 << TASK_TYPE);}
void cmdTaskSys(typeof(list_table->task_list[0]->status) *status){*status |= (1 << TASK_TYPE);}
void cmdTaskStart(typeof(list_table->task_list[0]->status) *status){*status |= (1 << TASK_START_AT_BOOT);}
void cmdTaskNoStart(typeof(list_table->task_list[0]->status) *status){*status &= ~(1 << TASK_START_AT_BOOT);}

cmd_t cmd_task[] = {
	{"-user", cmdTaskUser},
	{"-sys", cmdTaskSys},
	{"-start@boot", cmdTaskStart},
	{"-nostart@boot", cmdTaskNoStart}	
};

void cmdDriverInit(typeof(list_table->driver_list[0]->status) *status){*status |= (1<<DRIVER_INIT_AT_BOOT);}
void cmdDriverNoInit(typeof(list_table->driver_list[0]->status) *status){*status &= ~(1<<DRIVER_INIT_AT_BOOT);}
void cmdDriverStart(typeof(list_table->driver_list[0]->status) *status){*status |= (1<<DRIVER_START_AT_BOOT);}
void cmdDriverNoStart(typeof(list_table->driver_list[0]->status) *status){*status &= ~(1<<DRIVER_START_AT_BOOT);}

cmd_t cmd_driver[] = {
	{"-init@boot", cmdDriverInit},
	{"-noinit@boot", cmdDriverNoInit},
	{"-start@boot", cmdDriverStart},
	{"-nostart@boot", cmdDriverNoStart}

};

int cmdTaskDispatch(const char *cmd, typeof(list_table->task_list[0]->status) *status)
{
	printf("[listCmd.c cmdTaskDispatch()] cmd=%s %i\n",cmd, *status);
	return 0;
}

int cmdDriverDispatch(const char *cmd, typeof(list_table->driver_list[0]->status) *status)
{
	printf("[listCmd.c cmdDriverDispatch()] cmd=%s %i\n",cmd, *status);
	return 0;
}
