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
 * @file autoCode.h
 * @brief autoCode header definitions
 *
 * @todo add run level count in module_t -> intrcCmdDispatch
 */

#ifndef AUTOCODE_H
#define AUTOCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get TaskMate define
#include "src/sysCore/modules_define.h"
#include "src/sysCore/run_level_define.h"

// message macro
#define msgError(msg)  fprintf(stderr, "[%s:%d] error : %s\n", __FILE_NAME__, __LINE__, msg)
#define msgInfo(msg)  fprintf(stdout, "[%s:%d] info : %s\n", __FILE_NAME__, __LINE__, msg)

typedef unsigned char module_status_t;
typedef int run_level_modules_count_t;

typedef struct
{
	char name[MODULES_NAME_SIZE_MAX];
	module_status_t status;
	int id;

} module_item_t;

typedef struct
{
	module_item_t services[MODULES_SERVICES_COUNT_MAX];
	int services_count;
	module_item_t tasks[MODULES_TASKS_COUNT_MAX];
	int tasks_count;
	module_item_t drivers[MODULES_DRIVERS_COUNT_MAX];
	int drivers_count;
	run_level_modules_count_t run_level_threads_count[RUN_LEVEL_COUNT];
	run_level_modules_count_t run_level_threads_total_count[RUN_LEVEL_COUNT];
	run_level_modules_count_t run_level_drivers_count[RUN_LEVEL_COUNT];


} module_t;

#endif
