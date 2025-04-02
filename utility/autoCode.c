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
 * - Write code for #include / static allocation / initialisation in initSys.c
 *
 * @note
 * tag format are one line C comment // [tag] <task|driver> <init>
 *
 * @warning
 * do not edit code between tag, it will be deleted by automatic generated code !
 * tasks file name must match with function, lcd.c -> void lcd(void),
 * drivers functions name must match to generic driver layout :
 * - <driver name>SetStatus()
 * - <driver name>GetStatus()
 * - <driver name>GetName()
 * - <driver name>Init()
 * - <driver name>Start()
 * - <driver name>Stop()
 *
 * @todo split autoCode.c -> many file/functions, read tag in list file to setup task/driver status
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get TaskMate flag bits
#include "/root/code/TaskMate/TaskMate_current/src/sysCore/status_bits.h"


// files name
#define FILE_TASK_LIST "utility/task_list"
#define FILE_DRIVER_LIST "utility/driver_list"
#define FILE_SOURCE "src/sysCore/initSys.c"
#define FILE_TEMP "src/sysCore/initSys.tmp.c"
#define FILE_TASK_INCLUDE "src/sysCore/autoIncludeTasks.h"
#define FILE_DRIVER_INCLUDE "src/sysCore/autoIncludeDrivers.h"
#define FILE_TASK_ALLOC "src/sysCore/autoAllocTasks.h"
#define FILE_DRIVER_ALLOC "src/sysCore/autoAllocDrivers.h"

// size of buffers
#define TASK_COUNT_MAX 256
#define DRIVER_COUNT_MAX 256

#define LINE_SIZE_MAX 256

#define ARGN_COUNT_MAX 4
#define ARGV_SIZE_MAX 64

// error message macro
#define ERRMSG(msg)  fprintf(stderr, "[%s:%d] error : %s\n", __FILE__, __LINE__, msg)

// struct for task/driver table
typedef struct
{
	char name[LINE_SIZE_MAX];
	unsigned char status;
	
} list_table_t;



// tokenizer
int getArg(char *line, int line_size_max, char **argv, int argn_count_max, int argv_size_max);

int main(void)
{
	//******************************************************************
	// allocation
	//******************************************************************
	
	// allocate task/driver tables
	list_table_t task_table[TASK_COUNT_MAX];
	list_table_t driver_table[DRIVER_COUNT_MAX];	
	
	// buffer for reading
	char *line; 
	int file_line_number;
	if( (line = malloc(LINE_SIZE_MAX * sizeof(*line))) == NULL)
		{ERRMSG("malloc line"); return(1);}
	
	// argument data
	int arg_count;
	char **argv; 
	if( (argv = malloc(ARGN_COUNT_MAX * sizeof(*argv))) == NULL)
		{ERRMSG("malloc argv\n"); return(1);}
	
	for(int i=0;i<ARGN_COUNT_MAX;i++)
	{
		if( (argv[i]=malloc(ARGV_SIZE_MAX * sizeof(**argv))) == NULL)
			{ERRMSG("malloc argv[]\n"); return(1);}
	}
	
	ERRMSG("no error, just a test");

	//******************************************************************
	// read list -> table
	//******************************************************************

	// open list files
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
		arg_count = getArg(line, LINE_SIZE_MAX, argv, ARGN_COUNT_MAX, ARGV_SIZE_MAX);
		
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
		driver_table[driver_count].status = (1 << DRIVER_INIT_AT_BOOT) | (1 << DRIVER_START_AT_BOOT); 
		
		file_line_number++;
		arg_count = getArg(line, LINE_SIZE_MAX, argv, ARGN_COUNT_MAX, ARGV_SIZE_MAX);

		if( (arg_count>0) && strcmp(argv[0],"#") ) //skip empty line or comment
		{
			if( (arg_count<1) | (arg_count>3) ) // test arg count
			{printf("[auroCode.c][driver_list] error : wrong arg count line %i\n",file_line_number);}
			else
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
	fclose(file_driver_list);

	//******************************************************************
	// print tables
	//******************************************************************
	int i;

	printf("[autoCode.c] found task :\n");
	for (i = 0; i < task_count; i++)
	{
		printf("\ttask[%i]=%s status=%i\n", i, task_table[i].name, task_table[i].status);
	}

	printf("\n[autoCode.c] found driver :\n");
	for (i = 0; i < driver_count; i++)
	{
		printf("\tdriver[%i]=%s status=%i\n", i, driver_table[i].name, driver_table[i].status);
	}
	printf("\n");

	//******************************************************************
	// write include files
	//******************************************************************

	// open include files
	FILE *file_task_include = fopen(FILE_TASK_INCLUDE, "w");
	if (file_task_include == NULL)
	{
		ERRMSG("creating file");
		printf("\t <%s>\n", FILE_TASK_INCLUDE);
		exit(1);
	}

	FILE *file_driver_include = fopen(FILE_DRIVER_INCLUDE, "w");
	if (file_driver_include == NULL)
	{
		ERRMSG("creating file");
		printf("\t <%s>\n", FILE_DRIVER_INCLUDE);
		exit(1);
	}

	// write task include
	for (i = 0; i < task_count; i++)
	{
		fprintf(file_task_include, "#include \"tasks/%s.h\"\n", task_table[i].name);
	}

	// write driver include
	for (i = 0; i < driver_count; i++)
	{
		fprintf(file_driver_include, "#include \"drivers/%s.h\"\n", driver_table[i].name);
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
		ERRMSG("creating file");
		printf("\t <%s>\n", FILE_TASK_ALLOC);
		exit(1);
	}

	FILE *file_driver_alloc = fopen(FILE_DRIVER_ALLOC, "w");
	if (file_driver_alloc == NULL)
	{
		ERRMSG("creating file");
		printf("\t  <%s>\n", FILE_DRIVER_ALLOC);
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
		ERRMSG("opening file");
		printf("\t <%s>\n", FILE_SOURCE);
		exit(1);
	}

	FILE *file_tmp = fopen(FILE_TEMP, "w");
	if (file_tmp == NULL)
	{
		ERRMSG("creating file");
		printf("\t <%s>\n", FILE_TEMP);
		exit(1);
	}

	// read form source
	int tag_section = 0;
	file_line_number = 0;
	
	while (fgets(line, LINE_SIZE_MAX, file_src))
	{
		file_line_number++;
		arg_count = getArg(line, LINE_SIZE_MAX, argv, ARGN_COUNT_MAX, ARGV_SIZE_MAX);

		
		if (!(strcmp(argv[0], "//")) && !(strcmp(argv[1], "[tag]")))
		{
			if (arg_count != 4)
			{
				ERRMSG("arg count != 4 line :");
				printf("\t %s:%i\n", FILE_SOURCE, file_line_number);
				break;
			}
						
			printf("found tag <%s> <%s> ... ", argv[2], argv[3]);
			fprintf(file_tmp, "%s", line);
			tag_section = 1;

			if (!(strcmp(argv[2], "task"))) // task tag
			{
				if (!(strcmp(argv[3], "init"))) // task init
				{
					for (i = 0; i < task_count; i++)
					{
						fprintf(file_tmp, "\ttaskCreate(%s, %i);\n",
						 task_table[i].name, i);
						
						fprintf(file_tmp, "\ttask_table[%i].task_name = \"%s\";\n", 
							i, task_table[i].name);
						fprintf(file_tmp, "\ttask_table[%i].setStatus = %sSetStatus;\n", 
							i, task_table[i].name);
						fprintf(file_tmp, "\ttask_table[%i].getStatus = %sGetStatus;\n", 
							i, task_table[i].name);		
						fprintf(file_tmp, "\t(*task_table[%i].setStatus)(%i);\n\n",
							i, task_table[i].status);
						
					}
				}
			}

			if (!(strcmp(argv[2], "driver"))) // driver tag
			{
				if (!(strcmp(argv[3], "init"))) // driver init
				{
					for (i = 0; i < driver_count; i++)
					{
						fprintf(file_tmp, "\tdriver_table[%i]=(driver_table_t)\n", i);
						fprintf(file_tmp, "\t{\n");
						fprintf(file_tmp, "\t\t.driver_id = %i,\n", i);
						fprintf(file_tmp, "\t\t.driver_name = \"%s\",\n", driver_table[i].name);
						fprintf(file_tmp, "\t\t.setStatus = %sSetStatus,\n", driver_table[i].name);
						fprintf(file_tmp, "\t\t.getStatus = %sGetStatus,\n", driver_table[i].name);
						fprintf(file_tmp, "\t\t.init = %sInit,\n", driver_table[i].name);
						fprintf(file_tmp, "\t\t.start = %sStart,\n", driver_table[i].name);
						fprintf(file_tmp, "\t\t.stop = %sStop\n", driver_table[i].name);
						fprintf(file_tmp, "\t};\n");
						fprintf(file_tmp, "\t(*driver_table[%i].setStatus)(%i);\n\n",
							i, driver_table[i].status);
					}
				}
			}
		}

		if (!(strcmp(argv[0], "//")) && !(strcmp(argv[1], "[/tag]")))
		{
			printf("end tag\n");
			tag_section = 0;
		}

		if (tag_section == 0)
		{
			fprintf(file_tmp, "%s", line);
		} 
	}

	if(tag_section == 1)
	{
		ERRMSG("missing end tag [/tag] at end of file");
	}
	
	// Replace original file with the modified version
	if (remove(FILE_SOURCE) != 0 || rename(FILE_TEMP, FILE_SOURCE) != 0)
	{
		ERRMSG("replacing initSys.c");
		exit(2);
	}

	fclose(file_src);
	fclose(file_tmp);

	for(int i=0;i<ARGN_COUNT_MAX;i++){free(argv[i]);}
	free(argv);
	free(line);
	
	return 0;
}

int getArg(char *line, int line_size_max, char **argv, int argn_count_max, int argv_size_max)
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
			&& (i_line < (line_size_max - 1)) && (argn < argn_count_max)	)	
	{
		// get off leading space or tab
		while (	((line[i_line] == ' ') || (line[i_line] == '\t')) 
				&& (i_line < line_size_max - 1) 	)
		{i_line++;}
		
		// read and store token
		i_arg = 0;
		while (	(line[i_line] != ' ') && (line[i_line] != '\t') 
				&& (line[i_line] != '\n') && (line[i_line] != 0) 
				&& (i_line < (line_size_max - 1)) && (i_arg < (argv_size_max-1))	)
		{argv[argn][i_arg++]=line[i_line++];}

		argv[argn][i_arg] = 0;
		if(argv[argn][0] != 0 ){argn++;}
	}

	return argn;
}

