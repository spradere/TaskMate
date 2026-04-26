/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseInitrc.c
 * @brief autoCode parseInitrc implementation.
 *
 */

#include "parseInitrc.h"

#include "initrcCmdDispatch.h"
#include "tokenizer.h"

void parseInitrc(modules_database_t *data_base, const char *file_initrc_name)
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
	tokenizer_t tok;
	module_item_t mod_tmp;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_initrc) )
	{
		// start
		file_line_number++;
		tokenizer(&tok);

		// process arguments
		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			if( tok.count != 3 )
			{
				msgError("wrong token count [%s:%i] is %i, should be 3",
						 file_initrc_name,
						 file_line_number,
						 tok.count);
				exit(1);
			}

			// parse commands
			for( int i = 1; i < tok.count; i++ )
			{
				int err = initrcCmdDispatch(tok.tokens[i], &mod_tmp);
				if( err == -1 )
				{
					msgError("unknown command [%s:%i] %s",
							 file_initrc_name,
							 file_line_number,
							 tok.tokens[i]);
					exit(1);
				}
			}

			// process name
			if( strlen(tok.tokens[0]) > TM_MOD_NAME_SIZE_MAX )
			{
				msgError("Name too long <%s> is over %i", tok.tokens[0], TM_MOD_NAME_SIZE_MAX);
				exit(1);
			}

			msgInfo("found module : %s\n", tok.tokens[0]);

			module_type_t *mod = &data_base->modules_type[mod_tmp.type];
			int module_count = mod->modules_count;

			// check duplicate name
			for( int j = 0; j < module_count; j++ )
			{
				if( strcmp(mod->modules[j].name, tok.tokens[0]) == 0 )
				{
					msgError("duplicate name [%s:%i] %s\n\n",
							 file_initrc_name,
							 file_line_number,
							 tok.tokens[0]);
					exit(1);
				}
			}

			// copy tmp mod in dest mod
			int index = mod->modules_count;
			if(index > AUTOCODE_MODULE_COUNT_MAX){msgError("too much modules > %i\n", index); exit(1);}

			strcpy(mod->modules[index].name, tok.tokens[0]);
			mod->modules[index].status = mod_tmp.status;
			mod->modules_count = index + 1;
		}
	}
	fclose(file_initrc);
}
