/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file initrcCmdDispatch.c
 * @brief autoCode initrcCmdDispatch implementation.
 *
 */

#include "initrcCmdDispatch.h"

static void funcRunNone(module_item_t *mod)
{
	mod->status |= RUN_NONE;
	mod->set_runlevel++;
}
static void funcRunCore(module_item_t *mod)
{
	mod->status |= RUN_CORE;
	mod->set_runlevel++;
}
static void funcRunDriver(module_item_t *mod)
{
	mod->status |= RUN_DRIVER;
	mod->set_runlevel++;
}
static void funcRunService(module_item_t *mod)
{
	mod->status |= RUN_SERVICE;
	mod->set_runlevel++;
}
static void funcRunUser(module_item_t *mod)
{
	mod->status |= RUN_USER;
	mod->set_runlevel++;
}
static void funcTypeDriver(module_item_t *mod)
{
	mod->type = TM_MOD_DRIVERS_ID;
	mod->set_type++;
}
static void funcTypeService(module_item_t *mod)
{
	mod->type = TM_MOD_THREAD_ID;
	mod->subtype = TM_MOD_THREAD_TYPE_SYS;
	mod->status |= (1 << TM_MOD_THREAD_TYPE_SYS);
	mod->set_type++;
}
static void funcTypeUser(module_item_t *mod)
{
	mod->type = TM_MOD_THREAD_ID;
	mod->subtype = TM_MOD_THREAD_TYPE_USER;
	mod->status |= (1 << TM_MOD_THREAD_TYPE_USER);
	mod->set_type++;
}

const initrc_cmd_t initrc_cmds[] = {{"-run_none", funcRunNone},
									{"-run_core", funcRunCore},
									{"-run_driver", funcRunDriver},
									{"-run_service", funcRunService},
									{"-run_user", funcRunUser},
									{"-type_driver", funcTypeDriver},
									{"-type_service", funcTypeService},
									{"-type_user", funcTypeUser},
									{NULL, NULL}};

int initrcCmdDispatch(const char *cmd, module_item_t *mod)
{
	for( int i = 0; initrc_cmds[i].name != NULL; i++ )
	{
		if( strcmp(cmd, initrc_cmds[i].name) == 0 )
		{
			(*initrc_cmds[i].func)(mod);
			return 0;
		}
	}
	return -1;
}
