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
 * @file parseInitrc.c
 * @brief read init.rc file and write data to modules
 *
 * @todo add check module -> file.c exist
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/parseInitrc.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/initrcCmdDispatch.h"

void parseInitrc(parse_init_rc_type_t TYPE, modules_database_t *data_base, const char *file_name)
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
	int err_flag = 0;
	tokenizer_t tok;
	module_type_t *mod = &data_base->modules_type[TYPE];
	int module_count = mod->modules_count;

	while( (module_count < AUTOCODE_MODULE_COUNT_MAX) && fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_initrc) )
	{
		// start
		mod->modules[module_count].status = mod->status_default;
		file_line_number++;
		tokenizer(&tok);

		// procced args
		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			if( tok.count > mod->initrc_arg_count_max )
			{
				msgError("wrong token count");
				printf("\t [%s:%i] is %i, should be max(%i)\n\n", file_name, file_line_number, tok.count,
					   mod->initrc_arg_count_max);
				err_flag = 1;
			}

			for( int j = 0; j < module_count; j++ )
			{
				if( strcmp(mod->modules[j].name, tok.tokens[0]) == 0 )
				{
					msgError("duplicate name");
					printf("\t [%s:%i] %s\n\n", file_name, file_line_number, tok.tokens[0]);
					err_flag = 1;
				}
			}

			// no cmd parsing, add one module to run level
			if( tok.count == 1 ) { data_base->run_level_module_count[TYPE][mod->status_default]++; }

			// parse commands
			for( int i = 1; i < tok.count; i++ )
			{
				int err = initrcCmdDispatch(tok.tokens[i], &mod->modules[module_count].status,
											&data_base->run_level_module_count[TYPE]);
				if( err != 0 )
				{
					msgError("unknown command");
					printf("\t [%s:%i] %s\n\n", file_name, file_line_number, tok.tokens[i]);
					err_flag = 1;
				}
			}

			// proceed name
			if( strlen(tok.tokens[0]) > MODULES_NAME_SIZE_MAX )
			{
				msgError("Name too long");
				printf("\t <%s> is over %i\n\n", tok.tokens[0], MODULES_NAME_SIZE_MAX);
				err_flag = 1;
			}

			strcpy(mod->modules[module_count].name, tok.tokens[0]);
			module_count++;
		}
	}

	if( module_count == 0 )
	{
		msgInfo("no module :");
		printf("\t in %s\n\n", file_name);
	}

	mod->modules_count = module_count;

	fclose(file_initrc);

	if( err_flag == 1 ) { exit(0); }
}
