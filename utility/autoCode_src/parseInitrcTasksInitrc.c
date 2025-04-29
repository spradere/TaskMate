/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file parseInitrcTasks.c
 * @brief read init.rc file and write data to modules
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/parseInitrc.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/initrcCmdDispatch.h"

void parseInitrcTasks(module_t *modules, char *file_name)
{
	// open list files
	msgInfo("open init.rc file  for parsing");
	printf("\t <%s> \n\n", file_name);

	FILE *file_initrc = fopen(file_name, "r");
	if( file_initrc == NULL )
	{
		msgError("file not found");
		exit(0);
	}

	// variables
	int file_line_number = 0;
	int err;
	tokenizer_t tok;
	int tasks_count = 0;

	while( (tasks_count < MODULE_TASK_COUNT_MAX) && fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_initrc) )
	{
		// set status to default
		modules->tasks[tasks_count].status = RUN_USER;

		file_line_number++;
		tokenizer(&tok);

		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			if( (tok.count < 1) | (tok.count > 2) )
			{
				msgError("wrong task token count");
				printf("\t [%s:%i] is %i, should be [1,2]\n\n", file_name, file_line_number, tok.count);
				exit(0);
			}

			for( int j = 0; j < tasks_count; j++ )
			{
				if( strcmp(modules->tasks[j].name, tok.tokens[0]) == 0 )
				{
					msgError("duplicate task name");
					printf("\t [%s:%i] %s\n\n", file_name, file_line_number, tok.tokens[0]);
					exit(0);
				}
			}

			// no cmd parsing, add one tread to run level
			if( tok.count == 1 ){modules->run_level_threads_count[RUN_USER]++;}

			for( int i = 1; i < tok.count; i++ )
			{
				err = initrcCmdDispatch(tok.tokens[i], &modules->tasks[tasks_count].status, modules->run_level_threads_count);
				if( err != 0 )
				{
					msgError("task unknown command");
					printf("\t [%s:%i] %s\n\n", file_name, file_line_number, tok.tokens[i]);
					exit(0);
				}
			}

			strcpy(modules->tasks[tasks_count].name, tok.tokens[0]);
			tasks_count++;
		}
	}

	if( tasks_count == 0 )
	{
		msgError("no task");
		printf("\t in %s\n\n", file_name);
		exit(0);
	}
	modules->tasks_count = tasks_count;

	// close files
	fclose(file_initrc);
}
