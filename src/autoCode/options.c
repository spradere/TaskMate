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
 * @file option.c
 * @brief autoCode option parser
 *
 */

#include "options.h"

#include "tokenizer.h"

static void funcArch(const char *value, options_list_t *opt)
{
	strncpy(opt->arch_name, value, BYTE_INDEX);
}

static void funcMcu(const char *value, options_list_t *opt)
{
	strncpy(opt->mcu_name, value, BYTE_INDEX);
}

static void funcBoard(const char *value, options_list_t *opt)
{
	strncpy(opt->board_name, value, BYTE_INDEX);
}

const options_cmd_t options_cmds[] = 	{{"--arch", funcArch},
										{"--mcu", funcMcu},
										{"--board", funcBoard},
										{NULL, NULL}};

static int optionCmdDispatch(const char *cmd, const char *value, options_list_t *opt)
{
	for( int i = 0; options_cmds[i].name != NULL; i++ )
	{
		if( strcmp(cmd, options_cmds[i].name) == 0 )
		{
			(*options_cmds[i].func)(value, opt);
			return 0;
		}
	}
	return -1;
}

void options(const char *file_name, options_list_t *opt)
{
	file_t file;
	fileInit(&file);
	file.name = (char*)file_name;
	fileOpen(&file,"r", __FILE__,__LINE__);

	int file_line_number = 0;
	tokenizer_t tok;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file.stream) )
	{
		file_line_number++;
		tokenizer(&tok);

		if(tok.count != 2)
		{
			msgError("Wrong token count [%s:%i] is %i, should be 1",
				file.name, file_line_number, tok.count);
			exit(1);
		}

		msgInfo(" parsing option <%s> value <%s>", tok.tokens[0], tok.tokens[1]);

		int err=optionCmdDispatch(tok.tokens[0], tok.tokens[1], opt);

		if(err != 0)
		{
			msgError("Unknow option [%s:%i] %s\n", file.name, file_line_number,tok.tokens[0]);
			exit(1);
		}
	}
}


