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

void listToTable(void)
{
	/*// open list files
	FILE *file_task_list = fopen(FILE_TASK_LIST, "r");
	if (file_task_list == NULL)
	{
		ERRMSG("task list file not found");
		printf("\t <%s>\n", FILE_TASK_LIST);
		return 1;
	}

	FILE *file_driver_list = fopen(FILE_DRIVER_LIST, "r");
	if (file_driver_list == NULL)
	{
		ERRMSG("driver list file not found");
		printf("\t <%s>\n", FILE_DRIVER_LIST);
		return 1;
	}

	// read task file -> table
	int task_count = 0;
	file_line_number = 0;

	while ((task_count < TASK_COUNT_MAX) &&
		   fgets(line, LINE_SIZE_MAX, file_task_list))
	{
		// set status to default
		task_table[task_count].status = 1 << TASK_START_AT_BOOT;

		file_line_number++;
		arg_count = tokenizer(line, LINE_SIZE_MAX, argv, ARGN_COUNT_MAX, ARGV_SIZE_MAX);

		if( (arg_count>0) && strcmp(argv[0],"#") ) //skip empty line or comment
		{
			if( (arg_count<2) | (arg_count>3) ) // test arg count
			{printf("[auroCode.c][task_list] error : wrong arg count line %i\n",file_line_number);}
			else
			{
				strcpy(task_table[task_count].name,argv[0]);
				task_count++;
			}
		}

	}
	if (task_count == 0)
	{
		ERRMSG("no task");
		return 1;
	}

	// read driver file -> table
	int driver_count = 0;
	file_line_number = 0;

	while ((driver_count < DRIVER_COUNT_MAX) &&
		   fgets(line, LINE_SIZE_MAX, file_driver_list))
	{
		// set status to default
		driver_table[driver_count].status = (1 << DRIVER_INIT_AT_BOOT) | (1 <<
	DRIVER_START_AT_BOOT);

		file_line_number++;
		arg_count = tokenizer(line, LINE_SIZE_MAX, argv, ARGN_COUNT_MAX, ARGV_SIZE_MAX);

		if( (arg_count>0) && strcmp(argv[0],"#") ) //skip empty line or comment
		{
			if( (arg_count<1) | (arg_count>3) ) // test arg count
			{printf("[auroCode.c][driver_list] error : wrong arg count line
	%i\n",file_line_number);} else
			{
				strcpy(driver_table[driver_count].name,argv[0]);
				driver_count++;
			}
		}

	}
	if (driver_count == 0)
	{
		ERRMSG("no drivers");
		return 1;
	}

	// close files
	fclose(file_task_list);
	fclose(file_driver_list);*/
}
