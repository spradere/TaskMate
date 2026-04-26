/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file autoCode.h
 * @brief autoCode header declarations.
 *
 */

#ifndef AUTOCODE_H
#define AUTOCODE_H

#define BYTE_INDEX 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// get TaskMate define
#define TM_SYSTEM_CRITICAL_ALLOWED
#include "interfaces/modules_define.h"
#include "sysCore/runLevel.h"

// message macro
#define msgError(format, ...) \
	fprintf(stderr, "[%s:%d] error : " format "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); perror("\t")

#define msgInfo(format, ...) \
	fprintf(stdout, "[%s] info : " format "\n", __FILE_NAME__, ##__VA_ARGS__)

typedef unsigned char module_status_t;
typedef int run_level_modules_count_t;

// modules structures
typedef struct
{
	char name[TM_MOD_NAME_SIZE_MAX];
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
	module_type_t modules_type[TM_MOD_TYPE_COUNT];
	run_level_modules_count_t run_level_module_count[TM_MOD_TYPE_COUNT][RUN_LEVEL_COUNT];
	int threads_count[RUN_LEVEL_COUNT];

} modules_database_t;

#endif
