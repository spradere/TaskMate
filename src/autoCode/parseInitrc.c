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
 * @brief read init.rc file and write data to modules database
 *
 */

#include "parseInitrc.h"

#include "initrcCmdDispatch.h"
#include "tokenizer.h"

void parseInitrc(const int TYPE, modules_database_t *data_base, const char *file_initrc_name)
{
	// open list files
	msgInfo("open <%s>", file_initrc_name);

	FILE *file_initrc = fopen(file_initrc_name, "r");
	if( file_initrc == NULL )
	{
		msgError("file not found <%s>", file_initrc_name);
		exit(1);
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

		// proceed arguments
		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			if( tok.count > mod->initrc_arg_count_max )
			{
				msgError("wrong token count [%s:%i] is %i, should be max(%i)", file_initrc_name,
						 file_line_number, tok.count, mod->initrc_arg_count_max);
				err_flag = 1;
			}

			for( int j = 0; j < module_count; j++ )
			{
				if( strcmp(mod->modules[j].name, tok.tokens[0]) == 0 )
				{
					msgError("duplicate name [%s:%i] %s\n\n", file_initrc_name, file_line_number,
							 tok.tokens[0]);
					err_flag = 1;
				}
			}

			// no command parsing, add one module to run level
			if( tok.count == 1 ) { data_base->run_level_module_count[TYPE][mod->status_default]++; }

			// parse commands
			for( int i = 1; i < tok.count; i++ )
			{
				int err = initrcCmdDispatch(tok.tokens[i], &mod->modules[module_count].status,
											&data_base->run_level_module_count[TYPE]);
				if( err != 0 )
				{
					msgError("unknown command [%s:%i] %s", file_initrc_name, file_line_number, tok.tokens[i]);
					err_flag = 1;
				}
			}

			// proceed name
			if( strlen(tok.tokens[0]) > MOD_NAME_SIZE_MAX )
			{
				msgError("Name too long <%s> is over %i", tok.tokens[0], MOD_NAME_SIZE_MAX);
				err_flag = 1;
			}

			strcpy(mod->modules[module_count].name, tok.tokens[0]);
			module_count++;
		}
	}

	msgInfo("found %i module", module_count);

	mod->modules_count = module_count;

	fclose(file_initrc);

	if( err_flag == 1 ) { exit(1); }
}
