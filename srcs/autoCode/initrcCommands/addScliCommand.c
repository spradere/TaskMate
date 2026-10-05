/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file addScliCommand.c
 * @brief init.rc addScliCommand command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "addScliCommand.h"

#include <sys/stat.h>

#define ADDSCLICOMMAND_COUNT_TOKENS 4U
#define ADDSCLICOMMAND_INDEX_OPTION 2U
#define ADDSCLICOMMAND_INDEX_SOURCE 3U

/* =============================================================================
 * Declarations - Local types
 * ===========================================================================*/

typedef struct
{
	const char *name;
	const char *function;
} scli_command_definition_t;

/* -----------------------------------------------
 * Supported SCLI commands
 * ---------------------------------------------*/

static const scli_command_definition_t scli_command_definitions[] = {
	{"date", "dateCommand"},
	{"driver", "driverCommand"},
	{"i2c", "i2cCommand"},
	{"stack", "stackCommand"},
	{"thread", "threadCommand"},
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static bool commandHeaderExists(const initrc_command_t *command, const char *name)
{
	char path[AUTOCODE_SIZE_BUFFER];
	const int length = snprintf(
		path, sizeof(path), "%s/system/services/commands/scli_%s.h", command->source_path, name);
	if( (length < (int)0U) || ((size_t)length >= sizeof(path)) ) { return false; }

	struct stat status;
	return (stat(path, &status) == 0U) && S_ISREG(status.st_mode);
}

static bool commandSourceFileExists(const initrc_command_t *command, const char *source_file)
{
	char path[AUTOCODE_SIZE_BUFFER];
	const int length = snprintf(path, sizeof(path), "%s/%s", command->source_path, source_file);
	if( (length < (int)0U) || ((size_t)length >= sizeof(path)) ) { return false; }

	struct stat status;
	return (stat(path, &status) == 0U) && S_ISREG(status.st_mode);
}

static const scli_command_definition_t *commandDefinitionFind(const char *name)
{
	for( size_t i = 0U; i < (sizeof(scli_command_definitions) / sizeof(scli_command_definitions[0U]));
		 i++ )
	{
		if( strcmp(scli_command_definitions[i].name, name) == 0U )
		{
			return &scli_command_definitions[i];
		}
	}
	return NULL;
}

/*
 * Accept only commands supported by the generated dispatch table and present in the source tree.
 * The database changes only after all checks pass.
 */
void initrcAddScliCommand(const initrc_command_t *command)
{
	// Validate the fixed command shape before indexing its tokens.
	if( command->tok->count != ADDSCLICOMMAND_COUNT_TOKENS )
	{
		AUTOCODE_MSG_ERROR("addScliCommand token count [%s:%i] is %i, should be %u",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count, ADDSCLICOMMAND_COUNT_TOKENS);
		return;
	}
	if( strcmp(command->tok->tokens[ADDSCLICOMMAND_INDEX_OPTION], "-source_file") != 0U )
	{
		AUTOCODE_MSG_ERROR("addScliCommand unknown option [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[ADDSCLICOMMAND_INDEX_OPTION]);
		return;
	}

	// A generated call requires a known name, its header, and its declared source.
	const char *name = command->tok->tokens[1U];
	if( strlen(name) >= MOD_NAME_SIZE_MAX - 1U )
	{
		AUTOCODE_MSG_ERROR("SCLI command identifier is too long [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   name);
		return;
	}
	const scli_command_definition_t *definition = commandDefinitionFind(name);
	// Generated calls use this fixed mapping, so unknown names cannot enter the table.
	if( definition == NULL )
	{
		AUTOCODE_MSG_ERROR("unknown SCLI command [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   name);
		return;
	}
	if( commandHeaderExists(command, name) == false )
	{
		AUTOCODE_MSG_ERROR("SCLI command header not found [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   name);
		return;
	}
	if( commandSourceFileExists(command, command->tok->tokens[ADDSCLICOMMAND_INDEX_SOURCE]) == false )
	{
		AUTOCODE_MSG_ERROR("SCLI command source file not found [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[ADDSCLICOMMAND_INDEX_SOURCE]);
		return;
	}

	for( uint8_t i = 0U; i < command->data_base->scli.count; i++ )
	{
		// Duplicate names would make the generated command lookup ambiguous.
		if( strcmp(command->data_base->scli.commands[i].name, name) == 0U )
		{
			AUTOCODE_MSG_ERROR("duplicate SCLI command name [%s:%i] %s",
							   command->initrc_name,
							   command->file_line_number,
							   name);
			return;
		}
	}

	if( command->data_base->scli.count >= AUTOCODE_COUNT_SCLIMAX )
	{
		AUTOCODE_MSG_ERROR("too many SCLI commands [%s:%i] maximum is %i",
						   command->initrc_name,
						   command->file_line_number,
						   AUTOCODE_COUNT_SCLIMAX);
		return;
	}

	// Store the validated name and matching function as one new table entry.
	const uint8_t index = command->data_base->scli.count;
	snprintf(command->data_base->scli.commands[index].name,
			 sizeof(command->data_base->scli.commands[index].name),
			 "%s",
			 name);
	snprintf(command->data_base->scli.commands[index].function,
			 sizeof(command->data_base->scli.commands[index].function),
			 "%s",
			 definition->function);
	command->data_base->scli.count++;
	AUTOCODE_MSG_INFO("found SCLI command : %s", name);
}
