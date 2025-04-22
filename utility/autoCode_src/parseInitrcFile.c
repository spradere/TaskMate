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
 * @file parseInitrcFile.c
 * @brief read init.rc file and write data to modules
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/parseInitrcFile.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/cmdDispatch.h"




void parseInitrcFile(module_t *modules, char *file_name, modules_type_t type)
{
	// open list files
	msgInfo("open init.rc file");
	printf("\t <%s> type=%i\n\n", file_name,type);

	FILE *file_initrc = fopen(file_name, "r");
	if (file_initrc == NULL)
	{
		msgError("file not found");
		exit(0);
	}


	// variables
	int file_line_number = 0;
	int err;
	tokenizer_t tok;

	// parse for tasks
	int task_count = 0;

	if(type==MODULE_TYPE_TASK)
	{
		while (((*modules).tasks_count < MODULE_TASK_COUNT_MAX) && fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_initrc))
		{
			// set status to default
			(*modules).tasks[task_count].status = RUN_USER;

			file_line_number++;
			tokenizer(&tok);

			if ((tok.count > 0) && strcmp(tok.tokens[0], "#")) // skip empty line or comment
			{
				if ((tok.count < 1) | (tok.count > 2))
				{
					msgError("wrong task token count");
					printf("\t [%s:%i] is %i, should be [1,2]\n\n",
						file_name,file_line_number,tok.count);
				}

				else
				{
					for (int j = 0; j < task_count; j++)
					{
						if (strcmp((*modules).tasks[j].name, tok.tokens[0]) == 0)
						{
							msgError("duplicate task name");
							printf("\t [%s:%i] %s\n\n",
								file_name,file_line_number,tok.tokens[0]);
							break;
						}
					}
				}

				strcpy((*modules).tasks[task_count].name, tok.tokens[0]);
				task_count++;

				for (int i = 1; i < tok.count; i++)
				{
					err = cmdDispatch(tok.tokens[i], &(*modules).tasks[task_count].status);
					if (err != 0)
					{
						msgError("task unknown command");
						printf("\t [%s:%i] %s\n\n",
							file_name,file_line_number,tok.tokens[i]);
					}
				}
			}
		}

		if (task_count == 0)
		{
			msgError("no task");
			printf("\t in %s\n\n",file_name);
			exit(0);
		}
		(*modules).tasks_count = task_count;

	}



	// close files
	fclose(file_initrc);

}
