/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file autoCode.h
 * @brief auto code header declarations.
 *
 */

#ifndef AUTOCODE_AUTOCODE_H
#define AUTOCODE_AUTOCODE_H

#define BYTE_INDEX 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// get TaskMate define
#define TM_SYSTEM_CRITICAL_ALLOWED
#include "interfaces/modules_define.h"
#include "system/sysCore/runLevel.h"

// message macro
#define msgError(format, ...) \
	do { \
	fprintf(stderr, "[%s:%d] error : " format "\n", __FILE_NAME__, __LINE__, ##__VA_ARGS__); \
	perror("\t"); } \
	while(0)

#define msgInfo(format, ...) \
	fprintf(stdout, "[%s] info : " format "\n", __FILE_NAME__, ##__VA_ARGS__)

// modules structures
typedef struct
{
	char name[TM_MOD_NAME_SIZE_MAX];
	unsigned char status;
	unsigned char type;
	unsigned char subtype;
	int set_runlevel;
	int set_type;
	
} module_item_t;

typedef struct
{
	module_item_t modules[TM_MOD_COUNT_MAX];
	int modules_count;
	
} module_type_t;

typedef struct
{
	module_type_t modules_type[TM_MOD_TYPE_COUNT];
	int run_level_module_count[TM_MOD_TYPE_COUNT][RUN_LEVEL_COUNT];
	int threads_count[RUN_LEVEL_COUNT];

} modules_database_t;

#endif // AUTOCODE_AUTOCODE_H
