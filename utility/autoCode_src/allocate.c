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
 * @file allocate.c
 * @brief allocation implemetation
 *
 * @todo nothing
 */
 
 
#include "utility/autoCode_src/autoCode.h"


void allocate(list_table_t *table, char *line, char **argv)
{
	// task and driver table
	if( (table = malloc(sizeof(*table))) == NULL)
		{ERRMSG("malloc table"); exit(0);}
	if( (table->driver_list = malloc(DRIVER_COUNT_MAX*sizeof(*table->driver_list))) == NULL)
		{ERRMSG("malloc table->driver_list"); exit(0);}
	if( (table->task_list = malloc(TASK_COUNT_MAX*sizeof(*table->task_list))) == NULL)
		{ERRMSG("malloc table->task_list"); exit(0);}
	
	for(int i=0;i<DRIVER_COUNT_MAX;i++)
	{
		if( (table->driver_list[i] = malloc(sizeof(**table->driver_list))) == NULL)
			{ERRMSG("malloc table->driver_list[i]"); exit(0);}
			
		if( (table->driver_list[i]->name = malloc(NAME_SIZE_MAX*sizeof(*table->driver_list[i]->name))) == NULL)
			{ERRMSG("malloc table->driver_list[i]->name"); exit(0);}		
	}
	
	for(int i=0;i<TASK_COUNT_MAX;i++)
	{
		if( (table->task_list[i] = malloc(sizeof(**table->task_list))) == NULL)
			{ERRMSG("malloc table->task_list[i]"); exit(0);}
		
		if( (table->task_list[i]->name = malloc(NAME_SIZE_MAX*sizeof(*table->task_list[i]->name))) == NULL)
			{ERRMSG("malloc table->task_list[i]->name"); exit(0);}		
	}
	
	// buffer for reading
	if( (line = malloc(LINE_SIZE_MAX * sizeof(*line))) == NULL)
		{ERRMSG("malloc line"); exit(0);}
		
	// argv
	if( (argv = malloc(ARGN_COUNT_MAX * sizeof(*argv))) == NULL)
		{ERRMSG("malloc argv\n"); exit(0);}
	
	for(int i=0;i<ARGN_COUNT_MAX;i++)
	{
		if( (argv[i]=malloc(ARGV_SIZE_MAX * sizeof(**argv))) == NULL)
			{ERRMSG("malloc argv[]\n"); exit(0);}
	}
}
