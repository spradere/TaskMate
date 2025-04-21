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
 * @brief read list file and write to modules
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/listToTable.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/cmdDispatch.h"





void listToTable(module_t *modules, char *line, char **tokens)
{
	/*// open list files
	FILE *file_task_list = fopen(FILE_TASK_LIST, "r");
	if (file_task_list == NULL)
	{
		msgError("task list file not found");
		printf("\t <%s>\n\n", FILE_TASK_LIST);
		exit(0);
	}

	FILE *file_driver_list = fopen(FILE_DRIVER_LIST, "r");
	if (file_driver_list == NULL)
	{
		msgError("driver list file not found");
		printf("\t <%s>\n\n", FILE_DRIVER_LIST);
		exit(0);
	}

	// variables
	int file_line_number = 0;
	int token_count;
	int err;

	// read task file -> modules
	int task_count = 0;

	while ((modules->task_count < MODULE_TASK_COUNT_MAX) && fgets(line, LINE_SIZE_MAX, file_task_list))
	{
		// set status to default
		modules->tasks[task_count]->status = 1 << THREAD_START_AT_BOOT;

		file_line_number++;
		token_count = tokenizer(line, tokens);

		if ((token_count > 0) && strcmp(tokens[0], "#")) // skip empty line or comment
		{
			if ((token_count < 2) | (token_count > 3))
			{
				msgError("wrong task token count");
				printf("\t [%s:%i] is %i, should be [2,3]\n\n",
					FILE_TASK_LIST,file_line_number,token_count);
			}

			else
			{
				for (int j = 0; j < task_count; j++)
				{
					if (strcmp(modules->tasks[j]->name, tokens[0]) == 0)
					{
						msgError("duplicate task name");
						printf("\t [%s:%i] %s\n\n",
							FILE_TASK_LIST,file_line_number,tokens[0]);
						break;
					}
				}

				strcpy(modules->tasks[task_count]->name, tokens[0]);

				for (int i = 1; i < token_count; i++)
				{
					err = cmdTaskDispatch(tokens[i], &modules->tasks[task_count]->status);
					if (err != 0)
					{
						msgError("task unknown command");
						printf("\t [%s:%i] %s\n\n",
							FILE_TASK_LIST,file_line_number,tokens[i]);
					}
				}
				if ((modules->tasks[task_count]->status & THREAD_TYPE_MASK) == 0)
				{
					msgError("missing -user or -system for task");
					printf("\t [%s:%i] %s\n\n",
						FILE_TASK_LIST,file_line_number,tokens[0]);
				}
				if ((modules->tasks[task_count]->status & THREAD_TYPE_MASK) == THREAD_TYPE_MASK)
				{
					msgError("you can't -user and -system for task");
					printf("\t [%s:%i] %s\n\n",
						FILE_TASK_LIST,file_line_number,tokens[0]);
				}				task_count++;
			}
		}
	}

	if (task_count == 0)
	{
		msgError("no task");
		printf("\t in %s\n\n",FILE_TASK_LIST);
		exit(0);
	}
	modules->task_count = task_count;

	// read driver file -> modules
	int driver_count = 0;
	file_line_number = 0;

	while ((driver_count < MODULE_DRIVER_COUNT_MAX) && fgets(line, LINE_SIZE_MAX, file_driver_list))
	{
		// set driver status to default
		modules->drivers[driver_count]->status =
			(1 << DRIVER_INIT_AT_BOOT) | (1 << DRIVER_START_AT_BOOT);

		file_line_number++;
		token_count = tokenizer(line, tokens);

		if ((token_count > 0) && strcmp(tokens[0], "#")) // skip empty line or comment
		{
			if ((token_count < 1) | (token_count > 3))
			{
				msgError("wrong driver token count");
				printf("\t [%s:%i] is %i, should be [1,3]\n\n",
					FILE_DRIVER_LIST,file_line_number,token_count);
			}

			else
			{
				for (int j = 0; j < driver_count; j++)
				{
					if (strcmp(modules->drivers[j]->name, tokens[0]) == 0)
					{
						msgError("duplicate driver name");
						printf("\t [%s:%i] %s\n\n",
							FILE_DRIVER_LIST,file_line_number,tokens[0]);
						break;
					}
				}

				strcpy(modules->drivers[driver_count]->name, tokens[0]);
				for (int i = 1; i < token_count; i++)
				{
					err = cmdDriverDispatch(tokens[i], &modules->drivers[driver_count]->status);
					if (err != 0)
					{
						msgError("driver unknown command");
						printf("\t [%s:%i] %s\n\n",
							FILE_TASK_LIST,file_line_number,tokens[i]);
					}
				}
				driver_count++;
			}
		}
	}

	if (driver_count == 0)
	{
		msgError("no drivers");
		printf("\t in %s\n\n",FILE_DRIVER_LIST);
		exit(0);
	}
	modules->driver_count = driver_count;

	// close files
	fclose(file_task_list);
	fclose(file_driver_list);*/
}
