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
 * @todo implement run level
 */

#ifndef AUTOCODE_H
#define AUTOCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// files name
#define FILE_TASK_LIST "utility/tasks_list"
#define FILE_DRIVER_LIST "utility/drivers_list"
#define FILE_SOURCE "src/sysCore/initSys.c"
#define FILE_TEMP "src/sysCore/initSys.tmp.c"
#define FILE_TASK_INCLUDE "src/sysCore/autoIncludeTasks.h"
#define FILE_DRIVER_INCLUDE "src/sysCore/autoIncludeDrivers.h"
#define FILE_TASK_ALLOC "src/sysCore/autoAllocTasks.h"
#define FILE_DRIVER_ALLOC "src/sysCore/autoAllocDrivers.h"

// size for module table
#define MODULE_TASK_COUNT_MAX 128
#define MODULE_SERVICE_COUNT_MAX 128
#define MODULE_DRIVER_COUNT_MAX 256
#define MODULE_NAME_SIZE_MAX 32

// message macro
#define msgError(msg)  fprintf(stderr, "[%s:%d] error : %s\n", __FILE_NAME__, __LINE__, msg)
#define msgInfo(msg)  fprintf(stdout, "[%s:%d] info : %s\n", __FILE_NAME__, __LINE__, msg)

// structs for task/driver table

typedef unsigned char module_status_t;

typedef struct
{
	char name[MODULE_NAME_SIZE_MAX];
	module_status_t status;

} module_item_t;



typedef struct
{
	module_item_t services[MODULE_SERVICE_COUNT_MAX];
	int services_count;
	module_item_t tasks[MODULE_TASK_COUNT_MAX];
	int task_count;
	module_item_t drivers[MODULE_DRIVER_COUNT_MAX];
	int driver_count;

} module_t;

#endif
