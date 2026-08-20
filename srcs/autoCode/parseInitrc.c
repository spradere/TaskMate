/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file parseInitrc.c
 * @brief parse initrc implementation.
 *
 */

#include "parseInitrc.h"

#include "fileUtility.h"
#include "initrcCmdDispatch.h"
#include "tokenizer.h"

void parseInitrc(modules_database_t *data_base, const char *initrc_name)
{
	// open list files
	AUTOCODE_MSG_INFO("open <%s>", initrc_name);

	file_t initrc_list;
	fileInit(&initrc_list);
	initrc_list.name = (char *)initrc_name;
	fileOpen(&initrc_list, "r", FILE_READONLY, __FILE__, __LINE__);

	// variables
	int file_line_number = 0;
	tokenizer_t tok;
	module_item_t mod_tmp;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, initrc_list.stream) )
	{
		// start
		file_line_number++;
		tokenizer(&tok);

		// process arguments
		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) ) // skip empty line or comment
		{
			if( tok.count != 3 )
			{
				AUTOCODE_MSG_ERROR("wrong token count [%s:%i] is %i, should be 3",
								   initrc_name,
								   file_line_number,
								   tok.count);
				exit(1);
			}

			// reset tmp module
			mod_tmp.status = 0;
			mod_tmp.set_runlevel = 0;
			mod_tmp.set_type = 0;

			// parse commands
			for( int i = 1; i < tok.count; i++ )
			{
				int err = initrcCmdDispatch(tok.tokens[i], &mod_tmp);
				if( err == -1 )
				{
					AUTOCODE_MSG_ERROR(
						"unknown command [%s:%i] %s", initrc_name, file_line_number, tok.tokens[i]);
					exit(1);
				}
			}

			// process name
			if( strlen(tok.tokens[0]) > TM_MOD_NAME_SIZE_MAX )
			{
				AUTOCODE_MSG_ERROR(
					"Name too long <%s> is over %i", tok.tokens[0], TM_MOD_NAME_SIZE_MAX);
				exit(1);
			}

			AUTOCODE_MSG_INFO("found module : %s", tok.tokens[0]);

			// ok go on with selected type in initrc
			module_type_t *mod = &data_base->modules_type[mod_tmp.type];
			int module_count = mod->modules_count;

			// check duplicate name
			for( int j = 0; j < module_count; j++ )
			{
				if( strcmp(mod->modules[j].name, tok.tokens[0]) == 0 )
				{
					AUTOCODE_MSG_ERROR("duplicate name [%s:%i] %s\n\n",
									   initrc_name,
									   file_line_number,
									   tok.tokens[0]);
					exit(1);
				}
			}

			// check options
			if( mod_tmp.set_runlevel < 1 )
			{
				AUTOCODE_MSG_ERROR("Module %s : -run_* option is not set", tok.tokens[0]);
				exit(1);
			}
			if( mod_tmp.set_runlevel > 1 )
			{
				AUTOCODE_MSG_ERROR("Module %s : -run_* option is multiple set", tok.tokens[0]);
				exit(1);
			}
			if( mod_tmp.set_type < 1 )
			{
				AUTOCODE_MSG_ERROR("Module %s : -type_* option is not set", tok.tokens[0]);
				exit(1);
			}
			if( mod_tmp.set_type > 1 )
			{
				AUTOCODE_MSG_ERROR("Module %s : -type_* option is multiple set", tok.tokens[0]);
				exit(1);
			}

			// copy tmp mod in dest mod
			int index = mod->modules_count;
			if( index > TM_MOD_COUNT_MAX )
			{
				AUTOCODE_MSG_ERROR("too much modules > %i type=%i\n", index, mod_tmp.type);
				exit(1);
			}

			strcpy(mod->modules[index].name, tok.tokens[0]);
			mod->modules[index].status = mod_tmp.status;
			mod->modules_count = index + 1;
			mod->modules[index].subtype = mod_tmp.subtype;
		}
	}
	fileClose(&initrc_list, __FILE__, __LINE__);
}
