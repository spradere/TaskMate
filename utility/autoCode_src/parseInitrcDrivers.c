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
 * @file parseInitrcDrivers.c
 * @brief read init.rc file and write data to modules
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/parseInitrc.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/initrcCmdDispatch.h"

void parseInitrcDrivers(module_t *modules, char *file_name)
{
	// open list files
	msgInfo("open init.rc file for parsing");
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
	int drivers_count = 0;

	while( (drivers_count < MODULE_DRIVER_COUNT_MAX) && fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_initrc) )
	{
		// set status to default
		modules->drivers[drivers_count].status = RUN_DRIVER;

		file_line_number++;
		tokenizer(&tok);

		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			if( (tok.count < 1) | (tok.count > 2) )
			{
				msgError("wrong driver token count");
				printf("\t [%s:%i] is %i, should be [1,2]\n\n", file_name, file_line_number, tok.count);
				exit(0);
			}

			for( int j = 0; j < drivers_count; j++ )
			{
				if( strcmp(modules->drivers[j].name, tok.tokens[0]) == 0 )
				{
					msgError("duplicate driver name");
					printf("\t [%s:%i] %s\n\n", file_name, file_line_number, tok.tokens[0]);
					exit(0);
				}
			}

			// no cmd parsing, add one tread to run level
			if( tok.count == 1 ){modules->run_level_modules_count[RUN_DRIVER]++;}

			for( int i = 1; i < tok.count; i++ )
			{
				err = initrcCmdDispatch(tok.tokens[i], &modules->drivers[drivers_count].status, modules->run_level_modules_count);
				if( err != 0 )
				{
					msgError("driver unknown command");
					printf("\t [%s:%i] %s\n\n", file_name, file_line_number, tok.tokens[i]);
					exit(0);
				}
			}

			strcpy(modules->drivers[drivers_count].name, tok.tokens[0]);
			drivers_count++;
		}
	}

	if( drivers_count == 0 )
	{
		msgError("no driver");
		printf("\t in %s\n\n", file_name);
		exit(0);
	}
	modules->drivers_count = drivers_count;

	// close files
	fclose(file_initrc);
}
