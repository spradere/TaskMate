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
 * @todo nothing
 */

#ifndef AUTOCODE_H
#define AUTOCODE_H
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get TaskMate flag bits
#include "src/sysCore/status_bits.h"


// files name
#define FILE_TASK_LIST "utility/task_list"
#define FILE_DRIVER_LIST "utility/driver_list"
#define FILE_SOURCE "src/sysCore/initSys.c"
#define FILE_TEMP "src/sysCore/initSys.tmp.c"
#define FILE_TASK_INCLUDE "src/sysCore/autoIncludeTasks.h"
#define FILE_DRIVER_INCLUDE "src/sysCore/autoIncludeDrivers.h"
#define FILE_TASK_ALLOC "src/sysCore/autoAllocTasks.h"
#define FILE_DRIVER_ALLOC "src/sysCore/autoAllocDrivers.h"

// size for list table
#define TASK_COUNT_MAX 256
#define DRIVER_COUNT_MAX 256
#define NAME_SIZE_MAX 64

// size for tokenizer
#define LINE_SIZE_MAX 256
#define ARGN_COUNT_MAX 4
#define ARGV_SIZE_MAX 64

// error message macro
#define ERRMSG(msg)  fprintf(stderr, "[%s:%d] error : %s\n", __FILE__, __LINE__, msg)

// struct for task/driver table
typedef struct
{
	char *name;
	unsigned char status;
	
} item_table_t;

typedef struct
{
	item_table_t **task_list;
	int task_count;
	item_table_t **driver_list;
	int driver_count;
	
} list_table_t;

#endif
