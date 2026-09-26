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

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "parseInitrc.h"

#include "fileUtility.h"
#include "initrcCommands/addModule.h"
#include "initrcCommands/addScliCommand.h"
#include "initrcCommands/setVersion.h"
#include "tokenizer.h"

/* -----------------------------------------------
 * init.rc command dispatch table
 * ---------------------------------------------*/

#define INITRC_COMMAND(X)                         \
	X("addModule", initrcAddModule)              \
	X("addScliCommand", initrcAddScliCommand)

static const struct
{
	const char *name;
	void (*func)(const initrc_command_t *command);
} initrc_commands[] = {
#define X(name, func) {(name), (func)},
	INITRC_COMMAND(X)
#undef X
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static bool initrcCommandDispatch(const initrc_command_t *command)
{
	for( size_t i = 0; i < (sizeof(initrc_commands) / sizeof(initrc_commands[0])); i++ )
	{
		if( strcmp(command->tok->tokens[0], initrc_commands[i].name) == 0 )
		{
			(*initrc_commands[i].func)(command);
			return true;
		}
	}
	return false;
}

static bool initrcVersionCommand(const initrc_command_t *command,
								 const char *position,
								 const char *option,
								 const int expected_version)
{
	if( (strcmp(command->tok->tokens[0], "setVersion") != 0) ||
		(command->tok->count < 2) || (strcmp(command->tok->tokens[1], option) != 0) )
	{
		AUTOCODE_MSG_ERROR("%s init.rc command [%s:%i] must be setVersion %s %i",
						   position,
						   command->initrc_name,
						   command->file_line_number,
						   option,
						   expected_version);
		return false;
	}

	return initrcSetVersion(command);
}

int parseInitrc(modules_database_t *data_base, const char *initrc_name, const char *source_path)
{
	AUTOCODE_MSG_INFO("open <%s>", initrc_name);

	file_t initrc_list;
	fileInit(&initrc_list);
	initrc_list.name = (char *)initrc_name;
	if( fileOpen(&initrc_list, "r", FILE_READONLY, __FILE__, __LINE__) != 0 ) { return -1; }

	int file_line_number = 0;
	tokenizer_t tok = {0};
	file_get_line_result_t line_result;
	bool version_major_set = false;
	bool version_minor_set = false;
	bool version_invalid = false;

	while( (line_result = fileGetLine(&initrc_list, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		file_line_number++;
		if( tokenizer(&tok) != 0 )
		{
			if( (version_major_set == false) || (version_minor_set == false) )
			{
				version_invalid = true;
				break;
			}
			continue;
		}
		if( (tok.count == 0) || (strcmp(tok.tokens[0], "#") == 0) ) { continue; }

		const initrc_command_t command = {.data_base = data_base,
									 .tok = &tok,
									 .initrc_name = initrc_name,
									 .source_path = source_path,
									 .file_line_number = file_line_number};

		if( version_major_set == false )
		{
			version_major_set = initrcVersionCommand(
				&command, "first", "major", AC_INITRC_EXPECTED_VER_MAJOR);
			if( version_major_set == false )
			{
				version_invalid = true;
				break;
			}
			continue;
		}
		if( version_minor_set == false )
		{
			version_minor_set = initrcVersionCommand(
				&command, "second", "minor", AC_INITRC_EXPECTED_VER_MINOR);
			if( version_minor_set == false )
			{
				version_invalid = true;
				break;
			}
			continue;
		}
		if( strcmp(tok.tokens[0], "setVersion") == 0 )
		{
			AUTOCODE_MSG_ERROR("setVersion command after init.rc version declaration [%s:%i]",
							   initrc_name,
							   file_line_number);
			version_invalid = true;
			break;
		}
		if( initrcCommandDispatch(&command) == false )
		{
			AUTOCODE_MSG_ERROR(
				"unknown command [%s:%i] %s", initrc_name, file_line_number, tok.tokens[0]);
		}
	}

	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", initrc_list.name, file_line_number);
	}
	if( (version_invalid == false) && (version_major_set == false) )
	{
		AUTOCODE_MSG_ERROR("missing setVersion major %i as first command of init.rc file <%s>",
						   AC_INITRC_EXPECTED_VER_MAJOR,
						   initrc_name);
	}
	if( (version_invalid == false) && version_major_set && (version_minor_set == false) )
	{
		AUTOCODE_MSG_ERROR("missing setVersion minor %i as second command of init.rc file <%s>",
						   AC_INITRC_EXPECTED_VER_MINOR,
						   initrc_name);
	}
	tokenizerFree(&tok);
	int result = (line_result == FILE_GET_LINE_ERROR) ? -1 : 0;
	if( fileClose(&initrc_list, __FILE__, __LINE__) != 0 ) { result = -1; }
	return result;
}
