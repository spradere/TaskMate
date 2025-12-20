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
 */

#ifndef AUTOCODE_H
#define AUTOCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get TaskMate define
#include "src/sysCore/modules_define.h"
#include "src/sysCore/runLevel.h"

// message macro
#define msgError(format, ...) \
	fprintf(stderr, "[%s:%d] error : " format "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); perror("\t")

#define msgInfo(format, ...) \
	fprintf(stdout, "[%s] info : " format "\n", __FILE_NAME__, ##__VA_ARGS__)

// target name struture
typedef struct
{
	const char * const arch_name;
	const char * const mcu_name;
	const char * const board_name;
} target_t;

typedef unsigned char module_status_t;
typedef int run_level_modules_count_t;

// modules structures
typedef struct
{
	char name[MODULES_NAME_SIZE_MAX];
	module_status_t status;

} module_item_t;

#define AUTOCODE_MODULE_COUNT_MAX 256

typedef struct
{
	module_item_t modules[AUTOCODE_MODULE_COUNT_MAX];
	int modules_count;
	int initrc_arg_count_max;
	module_status_t status_default;
	char *name;

} module_type_t;

typedef struct
{
	module_type_t modules_type[MODULES_TYPE_COUNT];
	run_level_modules_count_t run_level_module_count[MODULES_TYPE_COUNT][RUN_LEVEL_COUNT];
	int threads_count[RUN_LEVEL_COUNT];

} modules_database_t;

#endif
