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
#include "initrcCommands/setVersion.h"
#include "tokenizer.h"

/* -----------------------------------------------
 * init.rc command dispatch table
 * ---------------------------------------------*/

#define INITRC_COMMAND(X)                     \
	X("setVersion", initrcSetVersion)        \
	X("addModule", initrcAddModule)

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
	initrc_version_state_t version_state = AC_INITRC_VERSION_EXPECT_MAJOR;

	while( (line_result = fileGetLine(&initrc_list, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		file_line_number++;
		if( tokenizer(&tok) != 0 )
		{
			if( version_state != AC_INITRC_VERSION_VALID )
			{
				version_state = AC_INITRC_VERSION_INVALID;
				break;
			}
			continue;
		}

		const initrc_command_t command = {.data_base = data_base,
									 .tok = &tok,
									 .initrc_name = initrc_name,
									 .source_path = source_path,
									 .file_line_number = file_line_number,
									 .version_state = &version_state};

		if( (version_state != AC_INITRC_VERSION_VALID) &&
			((tok.count == 0) || (strcmp(tok.tokens[0], "setVersion") != 0)) )
		{
			if( version_state == AC_INITRC_VERSION_EXPECT_MAJOR )
			{
				AUTOCODE_MSG_ERROR("first init.rc line [%s:%i] must be setVersion major %i",
								   initrc_name,
								   file_line_number,
								   AC_INITRC_EXPECTED_VER_MAJOR);
			}
			if( version_state == AC_INITRC_VERSION_EXPECT_MINOR )
			{
				AUTOCODE_MSG_ERROR("second init.rc line [%s:%i] must be setVersion minor %i",
								   initrc_name,
								   file_line_number,
								   AC_INITRC_EXPECTED_VER_MINOR);
			}
			version_state = AC_INITRC_VERSION_INVALID;
			break;
		}

		if( (tok.count > 0) && (strcmp(tok.tokens[0], "#") != 0) &&
			(initrcCommandDispatch(&command) == false) )
		{
			AUTOCODE_MSG_ERROR(
				"unknown command [%s:%i] %s", initrc_name, file_line_number, tok.tokens[0]);
		}
		if( version_state == AC_INITRC_VERSION_INVALID ) { break; }
	}

	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", initrc_list.name, file_line_number);
	}
	if( version_state == AC_INITRC_VERSION_EXPECT_MAJOR )
	{
		AUTOCODE_MSG_ERROR("missing setVersion major %i on first line of init.rc file <%s>",
						   AC_INITRC_EXPECTED_VER_MAJOR,
						   initrc_name);
	}
	if( version_state == AC_INITRC_VERSION_EXPECT_MINOR )
	{
		AUTOCODE_MSG_ERROR("missing setVersion minor %i after init.rc major version <%s>",
						   AC_INITRC_EXPECTED_VER_MINOR,
						   initrc_name);
	}
	tokenizerFree(&tok);
	int result = (line_result == FILE_GET_LINE_ERROR) ? -1 : 0;
	if( fileClose(&initrc_list, __FILE__, __LINE__) != 0 ) { result = -1; }
	return result;
}
