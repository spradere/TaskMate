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

static bool identifierCharacterIsValid(const char character)
{
	return ((character >= 'a') && (character <= 'z')) ||
		   ((character >= 'A') && (character <= 'Z')) ||
		   ((character >= '0') && (character <= '9')) || (character == '_');
}

static bool identifierIsValid(const char *identifier)
{
	if( !(((identifier[0] >= 'a') && (identifier[0] <= 'z')) ||
		  ((identifier[0] >= 'A') && (identifier[0] <= 'Z')) || (identifier[0] == '_')) )
	{
		return false;
	}

	for( size_t i = 1; identifier[i] != 0; i++ )
	{
		if( identifierCharacterIsValid(identifier[i]) == false ) { return false; }
	}
	return true;
}

static bool commandHeaderExists(const initrc_command_t *command, const char *name)
{
	char path[AC_BUFFER_SIZE];
	const int length = snprintf(path,
							  sizeof(path),
							  "%s/system/services/commands/%s.h",
							  command->source_path,
							  name);
	if( (length < 0) || ((size_t)length >= sizeof(path)) ) { return false; }

	struct stat status;
	return (stat(path, &status) == 0) && S_ISREG(status.st_mode);
}

static const scli_command_definition_t *commandDefinitionFind(const char *name)
{
	for( size_t i = 0;
		 i < (sizeof(scli_command_definitions) / sizeof(scli_command_definitions[0]));
		 i++ )
	{
		if( strcmp(scli_command_definitions[i].name, name) == 0 )
		{
			return &scli_command_definitions[i];
		}
	}
	return NULL;
}

void initrcAddScliCommand(const initrc_command_t *command)
{
	if( command->tok->count != 2 )
	{
		AUTOCODE_MSG_ERROR("addScliCommand token count [%s:%i] is %i, should be 2",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count);
		return;
	}

	const char *name = command->tok->tokens[1];
	if( identifierIsValid(name) == false )
	{
		AUTOCODE_MSG_ERROR("invalid SCLI command identifier [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   name);
		return;
	}
	if( autoCodeBufferStringFits(name, AC_SCLI_COMMAND_NAME_SIZE_MAX) == false )
	{
		AUTOCODE_MSG_ERROR("SCLI command identifier is too long [%s:%i] %s",
						   command->initrc_name,
						   command->file_line_number,
						   name);
		return;
	}
	const scli_command_definition_t *definition = commandDefinitionFind(name);
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

	for( uint8_t i = 0; i < command->data_base->scli.count; i++ )
	{
		if( strcmp(command->data_base->scli.commands[i].name, name) == 0 )
		{
			AUTOCODE_MSG_ERROR("duplicate SCLI command name [%s:%i] %s",
							   command->initrc_name,
							   command->file_line_number,
							   name);
			return;
		}
	}

	if( command->data_base->scli.count >= AC_SCLI_COMMAND_COUNT_MAX )
	{
		AUTOCODE_MSG_ERROR("too many SCLI commands [%s:%i] maximum is %i",
						   command->initrc_name,
						   command->file_line_number,
						   AC_SCLI_COMMAND_COUNT_MAX);
		return;
	}

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
