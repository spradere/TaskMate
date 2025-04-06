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
 * @file listToTable.c
 * @brief read list file and write to table
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/listToTable.h"
#include "utility/autoCode_src/tokenizer.h"

void listToTable(list_table_t *table, char *line, char **argv)
{
	// open list files
	FILE *file_task_list = fopen(FILE_TASK_LIST, "r");
	if (file_task_list == NULL)
	{
		ERRMSG("task list file not found");
		printf("\t <%s>\n", FILE_TASK_LIST);
		exit(0);
	}

	FILE *file_driver_list = fopen(FILE_DRIVER_LIST, "r");
	if (file_driver_list == NULL)
	{
		ERRMSG("driver list file not found");
		printf("\t <%s>\n", FILE_DRIVER_LIST);
		exit(0);
	}
	
	// variables
	int file_line_number = 0;
	int arg_count;

	// read task file -> table
	int task_count = 0;
	
	while ((table->task_count < TASK_COUNT_MAX) &&
		   fgets(line, LINE_SIZE_MAX, file_task_list))
	{
		// set status to default
		table->task_list[task_count]->status = 1 << TASK_START_AT_BOOT;

		file_line_number++;
		arg_count = tokenizer(line, argv);

		if( (arg_count>0) && strcmp(argv[0],"#") ) //skip empty line or comment
		{
			if( (arg_count<2) | (arg_count>3) ) // test arg count
			{printf("[auroCode.c][task_list] error : wrong arg count line %i\n",file_line_number);}
			
			else
			{strcpy(table->task_list[task_count++]->name,argv[0]);}
		}

	}
	
	if (task_count == 0){ERRMSG("no task"); exit(0);}
	table->task_count = task_count;

	// read driver file -> table
	int driver_count = 0;
	file_line_number = 0;
	
	while ((driver_count < DRIVER_COUNT_MAX) &&
		   fgets(line, LINE_SIZE_MAX, file_driver_list))
	{
		// set status to default
		table->driver_list[driver_count]->status = 
				(1 << DRIVER_INIT_AT_BOOT) | (1 << DRIVER_START_AT_BOOT);

		file_line_number++;
		arg_count = tokenizer(line, argv);

		if( (arg_count>0) && strcmp(argv[0],"#") ) //skip empty line or comment
		{
			if( (arg_count<1) | (arg_count>3) ) // test arg count
			{printf("[auroCode.c][driver_list] error : wrong arg count line%i\n",file_line_number);} 
			
			else
			{strcpy(table->driver_list[driver_count++]->name,argv[0]);}
		}

	}
	
	if (driver_count == 0){ERRMSG("no drivers"); exit(0);}
	table->driver_count = driver_count;

	// close files
	fclose(file_task_list);
	fclose(file_driver_list);
}
