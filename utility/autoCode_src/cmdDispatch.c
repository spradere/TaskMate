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
 * @file cmdDispatch.c
 * @brief command parser for list file options
 *
 * @todo check mandatory user/system
 */

#include <string.h>
#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/cmdDispatch.h"
// get TaskMate run levels
#include "src/sysCore/modules_define.h"

void cmdRunNone(module_status_t *status) 	{ *status &= RUN_LEVEL_MASK; *status |= RUN_NONE; }
void cmdRunCore(module_status_t *status) 	{ *status &= RUN_LEVEL_MASK; *status |= RUN_CORE; }
void cmdRunDriver(module_status_t *status) 	{ *status &= RUN_LEVEL_MASK; *status |= RUN_DRIVER; }
void cmdRunService(module_status_t *status) { *status &= RUN_LEVEL_MASK; *status |= RUN_SERVICE; }
void cmdRunUser(module_status_t *status) 	{ *status &= RUN_LEVEL_MASK; *status |= RUN_USER; }


const cmd_t cmdRunlevel[] = {{"-RUN_NONE", cmdRunNone},
							{"-RUN_CORE", cmdRunCore},
							{"-RUN_DRIVER", cmdRunDriver},
							{"-RUN_SERVICE", cmdRunService},
							{"-RUN_USER", cmdRunUser},
							{NULL, NULL}

};


int cmdDispatch(const char *cmd, module_status_t *status)
{
	for (int i = 0; cmdRunlevel[i].name != NULL; i++)
	{
		if (strcmp(cmd, cmdRunlevel[i].name) == 0)
		{
			(*cmdRunlevel[i].func)(status);
			return 0;
		}
	}
	return -1;
}

