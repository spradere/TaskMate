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
 * @todo check mandatory user/system
 */

#include <string.h>
#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/listCmd.h"
// get TaskMate flag bits
#include "src/sysCore/status_bits.h"


void cmdTaskUser(status_t *status){*status |= (1 << TASK_TYPE_USER);}
void cmdTaskSys(status_t *status){*status |= (1 << TASK_TYPE_SYSTEM);}
void cmdTaskStart(status_t *status){*status |= (1 << TASK_START_AT_BOOT);}
void cmdTaskNoStart(status_t *status){*status &= ~(1 << TASK_START_AT_BOOT);}

const cmd_t cmd_task[] = {
	{"-user", cmdTaskUser},
	{"-system", cmdTaskSys},
	{"-start@boot", cmdTaskStart},
	{"-nostart@boot", cmdTaskNoStart},
	{NULL,NULL}
};

void cmdDriverInit(status_t *status){*status |= (1<<DRIVER_INIT_AT_BOOT);}
void cmdDriverNoInit(status_t *status){*status &= ~(1<<DRIVER_INIT_AT_BOOT);}
void cmdDriverStart(status_t *status){*status |= (1<<DRIVER_START_AT_BOOT);}
void cmdDriverNoStart(status_t *status){*status &= ~(1<<DRIVER_START_AT_BOOT);}

const cmd_t cmd_driver[] = {
	{"-init@boot", cmdDriverInit},
	{"-noinit@boot", cmdDriverNoInit},
	{"-start@boot", cmdDriverStart},
	{"-nostart@boot", cmdDriverNoStart},
	{NULL,NULL}

};

int cmdTaskDispatch(const char *cmd, status_t *status)
{
	for(int i=0; cmd_task[i].name != NULL; i++)
		{
			if(strcmp(cmd,cmd_task[i].name) == 0)
			{
				(*cmd_task[i].func)(status);
				//printf("[listCmd.c cmdTaskDispatch()] cmd=%s status=0x%02x\n",cmd, *status);
				return 0;
			}
		}
	return -1;
}

int cmdDriverDispatch(const char *cmd, status_t *status)
{
	for(int i=0; cmd_driver[i].name != NULL; i++)
		{
			if(strcmp(cmd,cmd_driver[i].name) == 0)
			{
				(*cmd_driver[i].func)(status);
				//printf("[listCmd.c cmdDriverDispatch()] cmd=%s status=0x%02x\n",cmd, *status);
				return 0;
			}
		}
	return -1;
}
