/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file initrcCmdDispatch.c
 * @brief command parser for list file options
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/initrcCmdDispatch.h"

void funcRunNone(module_status_t *status, run_level_threads_count_t *count)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_NONE;
	count[RUN_NONE] = 0;
}
void funcRunCore(module_status_t *status, run_level_threads_count_t *count)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_CORE;
	count[RUN_CORE]++;
}
void funcRunDriver(module_status_t *status, run_level_threads_count_t *count)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_DRIVER;
	count[RUN_DRIVER]++;

}
void funcRunService(module_status_t *status, run_level_threads_count_t *count)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_SERVICE;
	count[RUN_SERVICE]++;
}
void funcRunUser(module_status_t *status, run_level_threads_count_t *count)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_USER;
	count[RUN_USER]++;
}

const initrc_cmd_t initrc_cmds[] = {{"-RUN_NONE", funcRunNone},
						  {"-RUN_CORE", funcRunCore},
						  {"-RUN_DRIVER", funcRunDriver},
						  {"-RUN_SERVICE", funcRunService},
						  {"-RUN_USER", funcRunUser},
						  {NULL, NULL}

};

int initrcCmdDispatch(const char *cmd, module_status_t *status, run_level_threads_count_t *level_count)
{
	for( int i = 0; initrc_cmds[i].name != NULL; i++ )
	{
		if( strcmp(cmd, initrc_cmds[i].name) == 0 )
		{
			(*initrc_cmds[i].func)(status, level_count);
			return 0;
		}
	}
	return -1;
}
