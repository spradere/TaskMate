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
#include "initrcCommands/setErrorCountMax.h"
#include "initrcCommands/setModuleCountMax.h"
#include "initrcCommands/setVersion.h"
#include "tokenizer.h"

enum { PARSEINITRC_COUNT_VERSIONMIN = 2U };

/* -----------------------------------------------
 * init.rc command dispatch table
 * ---------------------------------------------*/

#define INITRC_COMMAND(X)                     \
	X("addModule", initrcAddModule)           \
	X("addScliCommand", initrcAddScliCommand) \
	X("setModuleCountMax", initrcSetModuleCountMax) \
	X("setErrorCountMax", initrcSetErrorCountMax)

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
	for( size_t i = 0U; i < (sizeof(initrc_commands) / sizeof(initrc_commands[0U])); i++ )
	{
		if( strcmp(command->tok->tokens[0U], initrc_commands[i].name) == 0U )
		{
			(*initrc_commands[i].func)(command);
			return true;
		}
	}
	return false;
}

static bool initrcVersionCommand(const initrc_command_t *command, const char *position,
								 const char *option, const int expected_version)
{
	if( (strcmp(command->tok->tokens[0U], "setVersion") != 0U) ||
		(command->tok->count < PARSEINITRC_COUNT_VERSIONMIN) ||
		(strcmp(command->tok->tokens[1U], option) != 0U) )
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

/*
 * Require the two version commands before any declaration. Parsing errors are recorded by
 * autoCode; the return value reports file access failures.
 */
ac_result_t parseInitrc(modules_database_t *data_base, const char *initrc_name,
						const char *source_path)
{
	AUTOCODE_MSG_INFO("open <%s>", initrc_name);

	file_t initrc_list;
	fileInit(&initrc_list);
	initrc_list.name = (char *)initrc_name;
	if( fileOpen(&initrc_list, "r", FILEUTILITY_MODE_READONLY) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("opening file <%s>", initrc_name);
		return AC_RESULT_ERROR;
	}

	int file_line_number = 0U;
	tokenizer_t tok = {0U};
	file_get_line_result_t line_result;
	bool version_major_set = false;
	bool version_minor_set = false;
	bool version_invalid = false;

	// The first two commands establish the grammar before any module is accepted.
	while( (line_result = fileGetLine(&initrc_list, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		file_line_number++;
		// Keep malformed version lines from shifting the required command positions.
		tokenizer_err_t token_error = tokenizer(&tok);
		if( token_error != TOK_ERR_NOERR )
		{
			AUTOCODE_MSG_ERROR("tokenizer [%s:%i]: %s",
							   initrc_name,
							   file_line_number,
							   tokenizerErrorMessage(token_error));
			// A malformed version line cannot be skipped in favor of a later command.
			if( (version_major_set == false) || (version_minor_set == false) )
			{
				version_invalid = true;
				break;
			}
			continue;
		}
		if( (tok.count == 0U) || (strcmp(tok.tokens[0U], "#") == 0U) ) { continue; }

		const initrc_command_t command = {.data_base = data_base,
										  .tok = &tok,
										  .initrc_name = initrc_name,
										  .source_path = source_path,
										  .file_line_number = file_line_number};

		if( version_major_set == false )
		{
			version_major_set =
				initrcVersionCommand(&command, "first", "major", AUTOCODE_VERSION_INITRCMAJOR);
			if( version_major_set == false )
			{
				version_invalid = true;
				break;
			}
			continue;
		}
		if( version_minor_set == false )
		{
			version_minor_set =
				initrcVersionCommand(&command, "second", "minor", AUTOCODE_VERSION_INITRCMINOR);
			if( version_minor_set == false )
			{
				version_invalid = true;
				break;
			}
			continue;
		}
		if( strcmp(tok.tokens[0U], "setVersion") == 0U )
		{
			// Version declarations are valid only at the start of the file.
			AUTOCODE_MSG_ERROR("setVersion command after init.rc version declaration [%s:%i]",
							   initrc_name,
							   file_line_number);
			version_invalid = true;
			break;
		}
		// Only declarations reach normal dispatch after both version commands.
		if( initrcCommandDispatch(&command) == false )
		{
			AUTOCODE_MSG_ERROR(
				"unknown command [%s:%i] %s", initrc_name, file_line_number, tok.tokens[0U]);
		}
	}

	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", initrc_list.name, file_line_number);
	}
	// At EOF, distinguish missing version commands from malformed ones already reported.
	if( (version_invalid == false) && (version_major_set == false) )
	{
		AUTOCODE_MSG_ERROR("missing setVersion major %i as first command of init.rc file <%s>",
						   AUTOCODE_VERSION_INITRCMAJOR,
						   initrc_name);
	}
	if( (version_invalid == false) && version_major_set && (version_minor_set == false) )
	{
		AUTOCODE_MSG_ERROR("missing setVersion minor %i as second command of init.rc file <%s>",
						   AUTOCODE_VERSION_INITRCMINOR,
						   initrc_name);
	}
	if( (version_invalid == false) && version_major_set && version_minor_set &&
		(data_base->module_count_set == false) )
	{
		AUTOCODE_MSG_ERROR("missing setModuleCountMax in init.rc file <%s>", initrc_name);
	}
	if( (version_invalid == false) && version_major_set && version_minor_set &&
		(data_base->error_count_set == false) )
	{
		AUTOCODE_MSG_ERROR("missing setErrorCountMax in init.rc file <%s>", initrc_name);
	}
	tokenizerFree(&tok);
	ac_result_t result = (line_result == FILE_GET_LINE_ERROR) ? AC_RESULT_ERROR : AC_RESULT_OK;
	if( fileClose(&initrc_list) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("closing file <%s>", initrc_name);
		result = AC_RESULT_ERROR;
	}
	return result;
}
