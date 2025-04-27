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
 * @file cmdDispatch.c
 * @brief command parser for list file options
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/cmdDispatch.h"

void funcRunNone(module_status_t *status)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_NONE;
}
void funcRunCore(module_status_t *status)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_CORE;
}
void funcRunDriver(module_status_t *status)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_DRIVER;
}
void funcRunService(module_status_t *status)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_SERVICE;
}
void funcRunUser(module_status_t *status)
{
	*status &= RUN_LEVEL_MASK;
	*status |= RUN_USER;
}

const cmd_t cmd_list[] = {{"-RUN_NONE", funcRunNone},
						  {"-RUN_CORE", funcRunCore},
						  {"-RUN_DRIVER", funcRunDriver},
						  {"-RUN_SERVICE", funcRunService},
						  {"-RUN_USER", funcRunUser},
						  {NULL, NULL}

};

int cmdDispatch(const char *cmd, module_status_t *status)
{
	for( int i = 0; cmd_list[i].name != NULL; i++ )
	{
		if( strcmp(cmd, cmd_list[i].name) == 0 )
		{
			(*cmd_list[i].func)(status);
			return 0;
		}
	}
	return -1;
}
