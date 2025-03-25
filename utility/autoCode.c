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
 * @file autoCode.c
 * @brief helper to generate code for task and drivers handle in initSys.c
 *
 * - Simple and reliable, read plain text file <task_list> and <driver_list>
 * - Write code for #include / static allocation / initialisation
 *
 * @note
 * tag format are one line C comment // [tag] <task/driver> <init>
 *
 * @warning
 * do not edit code between tag it will be deleted by automatic generated code !
 * tasks file name must match with main function, lcd.c -> void lcd(void),
 * drivers functions name must match to generic driver layout :
 * - <driver name>SetStatus()
 * - <driver name>GetStatus()
 * - <driver name>GetName()
 * - <driver name>Init()
 * - <driver name>Start()
 * - <driver name>Stop()
 *
 * @todo change get_arg() -> less specific to tag handle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_TASK_LIST "utility/task_list"
#define FILE_DRIVER_LIST "utility/driver_list"
#define FILE_SOURCE "src/sysCore/initSys.c"
#define FILE_TEMP "src/sysCore/initSys.tmp.c"
#define FILE_TASK_INCLUDE "src/sysCore/autoIncludeTasks.h"
#define FILE_DRIVER_INCLUDE "src/sysCore/autoIncludeDrivers.h"
#define FILE_TASK_ALLOC "src/sysCore/autoAllocTasks.h"
#define FILE_DRIVER_ALLOC "src/sysCore/autoAllocDrivers.h"

#define TASK_COUNT_MAX 256
#define DRIVER_COUNT_MAX 256

#define LINE_SIZE_MAX 256

#define ARGN_COUNT_MAX 4
#define ARGV_SIZE_MAX 64

int getArg(char *line, int line_size_max, char *argv[][ARGV_SIZE_MAX], int argn_count_max, int argv_size_max);

int main(void)
{
	char line[LINE_SIZE_MAX]; // buffer for reading
	char argv[ARGN_COUNT_MAX][ARGV_SIZE_MAX]; // argument data

	//******************************************************************
	// read list -> table
	//******************************************************************
	// allocate tables
	char task_table[TASK_COUNT_MAX][LINE_SIZE_MAX];
	char driver_table[DRIVER_COUNT_MAX][LINE_SIZE_MAX];

	// open list files
	FILE *file_task_list = fopen(FILE_TASK_LIST, "r");
	if (file_task_list == NULL)
	{
		printf("[autoCode.c] error : task file not found <%s>\n", FILE_TASK_LIST);
		exit(0);
	}

	FILE *file_driver_list = fopen(FILE_DRIVER_LIST, "r");
	if (file_driver_list == NULL)
	{
		printf("[autoCode.c] error : driver file not found <%s>\n", FILE_DRIVER_LIST);
		exit(0);
	}

	// read task file -> table
	int task_count = 0;
	while ((task_count < TASK_COUNT_MAX) &&
		   fgets(task_table[task_count], LINE_SIZE_MAX, file_task_list))
	{
		task_table[task_count][strcspn(task_table[task_count], "\n")] = 0; // Remplace newline
		task_count++;
	}
	if (task_count == 0)
	{
		printf("[autoCode.c] error : no task\n");
		exit(0);
	}

	// read driver file -> table
	int driver_count = 0;
	while ((driver_count < DRIVER_COUNT_MAX) &&
		   fgets(driver_table[driver_count], LINE_SIZE_MAX, file_driver_list))
	{
		driver_table[driver_count][strcspn(driver_table[driver_count], "\n")] =
			0; // Remplace newline
		driver_count++;
	}
	if (driver_count == 0)
	{
		printf("[autoCode.c] error : no driver\n");
		exit(0);
	}

	// close files
	fclose(file_task_list);
	fclose(file_driver_list);

	//******************************************************************
	// print tables
	//******************************************************************
	int i;

	printf("[autoCode] found task :\n");
	for (i = 0; i < task_count; i++)
	{
		printf("\ttask[%i]=%s\n", i, task_table[i]);
	}

	printf("\n[autoCode] found driver :\n");
	for (i = 0; i < driver_count; i++)
	{
		printf("\tdriver[%i]=%s\n", i, driver_table[i]);
	}
	printf("\n");

	//******************************************************************
	// write include files
	//******************************************************************

	// open include files
	FILE *file_task_include = fopen(FILE_TASK_INCLUDE, "w");
	if (file_task_include == NULL)
	{
		printf("[autoCode.c] error : creating file  <%s>\n", FILE_TASK_INCLUDE);
		exit(1);
	}

	FILE *file_driver_include = fopen(FILE_DRIVER_INCLUDE, "w");
	if (file_driver_include == NULL)
	{
		printf("[autoCode.c] error : creating file  <%s>\n", FILE_DRIVER_INCLUDE);
		exit(1);
	}

	// write task include
	for (i = 0; i < task_count; i++)
	{
		fprintf(file_task_include, "#include \"tasks/%s.h\"\n", task_table[i]);
	}

	// write driver include
	for (i = 0; i < driver_count; i++)
	{
		fprintf(file_driver_include, "#include \"drivers/%s.h\"\n", driver_table[i]);
	}

	fclose(file_task_include);
	fclose(file_driver_include);

	//******************************************************************
	// write alloc files
	//******************************************************************

	// open include files
	FILE *file_task_alloc = fopen(FILE_TASK_ALLOC, "w");
	if (file_task_alloc == NULL)
	{
		printf("[autoCode.c] error : creating file  <%s>\n", FILE_TASK_ALLOC);
		exit(1);
	}

	FILE *file_driver_alloc = fopen(FILE_DRIVER_ALLOC, "w");
	if (file_driver_alloc == NULL)
	{
		printf("[autoCode.c] error : creating file  <%s>\n", FILE_DRIVER_ALLOC);
		exit(1);
	}

	// write task static alloc
	fprintf(file_task_alloc, "const uint8_t TASK_COUNT = %i;\n", task_count);
	fprintf(file_task_alloc, "task_table_t task_table[%i];\n", task_count);
	fprintf(file_task_alloc, "uint8_t task_current = 0;\n");

	// write driver static alloc
	fprintf(file_driver_alloc, "const uint8_t DRIVER_COUNT = %i;\n", driver_count);
	fprintf(file_driver_alloc, "driver_table_t driver_table[%i];\n", driver_count);

	fclose(file_task_alloc);
	fclose(file_driver_alloc);

	//******************************************************************
	// read tag to generate code
	//******************************************************************

	// open source and tmp file
	FILE *file_src = fopen(FILE_SOURCE, "r");
	if (file_src == NULL)
	{
		printf("[autoCode.c] error : source file not found <%s>\n", FILE_SOURCE);
		exit(1);
	}

	FILE *file_tmp = fopen(FILE_TEMP, "w");
	if (file_tmp == NULL)
	{
		printf("[autoCode.c] error : creating temp file  <%s>\n", FILE_TEMP);
		exit(1);
	}

	// read form source
	int tag_section = 0;
	int arg_count;
	int file_line_number = 0;

	while (fgets(*line, LINE_SIZE_MAX, file_src))
	{
		file_line_number++;
		arg_count = getArg(line, LINE_SIZE_MAX, argv, ARGN_COUNT_MAX, ARGV_SIZE_MAX);

		if (arg_count != 4)
		{
			printf("[autoCode] error in file <%s> line %i, arg count != 4/n", 
				FILE_SOURCE, file_line_number);
		}

		if (!(strcmp(*argv[0], "//")) && !(strcmp(*argv[1], "[tag]")))
		{
			printf("found tag <%s> <%s> ... ", *argv[2], *argv[3]);
			fprintf(file_tmp, "%s", *line);
			tag_section = 1;

			if (!(strcmp(*argv[2], "task"))) // task tag
			{
				if (!(strcmp(*argv[3], "init"))) // task init
				{
					fprintf(file_tmp, "\tuint8_t i = 0; \n");
					for (i = 0; i < task_count; i++)
					{
						fprintf(file_tmp, "\ttaskCreate(%s,i++);\n", task_table[i]);
					}
				}
			}

			if (!(strcmp(*argv[2], "driver"))) // driver tag
			{
				if (!(strcmp(*argv[3], "init"))) // driver init
				{
					for (i = 0; i < driver_count; i++)
					{
						fprintf(file_tmp, "\tdriver_table[%i]=(driver_table_t) \n", i);
						fprintf(file_tmp, "\t{\n");
						fprintf(file_tmp, "\t\t.driver_id = %i,\n", i);
						fprintf(file_tmp, "\t\t.driver_name = %sGetName(),\n", driver_table[i]);
						fprintf(file_tmp, "\t\t.setStatus = %sSetStatus, \n", driver_table[i]);
						fprintf(file_tmp, "\t\t.getStatus = %sGetStatus, \n", driver_table[i]);
						fprintf(file_tmp, "\t\t.init = %sInit, \n", driver_table[i]);
						fprintf(file_tmp, "\t\t.start = %sStart, \n", driver_table[i]);
						fprintf(file_tmp, "\t\t.stop = %sStop\n", driver_table[i]);
						fprintf(file_tmp, "\t};\n");
					}
				}
			}
		}

		if (!(strcmp(*argv[0], "//")) && !(strcmp(*argv[1], "[/tag]")))
		{
			printf("end tag\n");
			tag_section = 0;
		}

		if (tag_section == 0)
		{
			fprintf(file_tmp, "%s", *line);
		} // tag used to flush old code
	}

	// Replace original file with the modified version
	if (remove(FILE_SOURCE) != 0 || rename(FILE_TEMP, FILE_SOURCE) != 0)
	{
		perror("[autoCode.c] error : replacing initSys.c");
		exit(2);
	}

	fclose(file_src);
	fclose(file_tmp);

	return 0;
}

int getArg(char *line[], int line_size_max, char *argv[][ARGV_SIZE_MAX], int argn_count_max, int argv_size_max)
{
	int i_line = 0;
	int i_arg = 0;
	int argn;

	// reset all argv
	for (argn = 0; argn < argn_count_max; argn++)
	{
		argv[argn][0] = 0;
	}

	// read line, extract arguments
	argn=0;
	
	while (	(line[i_line] != '\n') && (line[i_line] != 0) 
			&& (i_line < (line_size_max - 1)) && (argn <= argn_count_max)	)	
	{
		// get off leading space or tab
		while (	((line[i_line] == ' ') || (line[i_line] == '\t')) 
				&& (line[i_line] != '\n') && (line[i_line] != 0) 
				&& (i_line < line_size_max - 1) 	)
		{i_line++;}
		
		// read and store data
		i_arg = 0;
		while (	(line[i_line] != ' ') && (line[i_line] == '\t') 
				&& (line[i_line] != '\n') && (line[i_line] != 0) 
				&& (i_line < (line_size_max - 1)) && (i_arg <= argv_size_max)	)
		{argv[argn][i_arg++]=line[i_line++];}
		
		argn++;
	}

	return argn;
}

