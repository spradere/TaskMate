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

// get TaskMate run levels
#include "src/sysCore/modules_define.h"

// message macro
#define msgError(msg)  fprintf(stderr, "[%s:%d] error : %s\n", __FILE_NAME__, __LINE__, msg)
#define msgInfo(msg)  fprintf(stdout, "[%s:%d] info : %s\n", __FILE_NAME__, __LINE__, msg)

// modules
#define MODULE_TASK_COUNT_MAX 128
#define MODULE_SERVICE_COUNT_MAX 128
#define MODULE_DRIVER_COUNT_MAX 256
#define MODULE_NAME_SIZE_MAX 32

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
	int tasks_count;
	module_item_t drivers[MODULE_DRIVER_COUNT_MAX];
	int drivers_count;

} module_t;

#endif
