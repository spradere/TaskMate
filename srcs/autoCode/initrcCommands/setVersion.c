/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setVersion.c
 * @brief init.rc setVersion command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "setVersion.h"

#include <errno.h>

/* -----------------------------------------------
 * Version option dispatch table
 * ---------------------------------------------*/

static void setVersionMajor(const initrc_command_t *command);
static void setVersionMinor(const initrc_command_t *command);

#define SET_VERSION_OPTION(X)              \
	X("major", setVersionMajor)           \
	X("minor", setVersionMinor)

static const struct
{
	const char *name;
	void (*func)(const initrc_command_t *command);
} set_version_options[] = {
#define X(name, func) {(name), (func)},
	SET_VERSION_OPTION(X)
#undef X
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static bool versionValueMatches(const char *value, const unsigned long expected)
{
	char *end;

	errno = 0;
	const unsigned long parsed = strtoul(value, &end, 10);
	if( (value[0] == 0) || (*end != 0) || (errno == ERANGE) ) { return false; }
	return parsed == expected;
}

static void setVersionMajor(const initrc_command_t *command)
{
	if( versionValueMatches(command->tok->tokens[2], AC_INITRC_EXPECTED_VER_MAJOR) == false )
	{
		AUTOCODE_MSG_ERROR("unsupported init.rc major syntax version [%s:%i] %s, expected %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[2],
						   AC_INITRC_EXPECTED_VER_MAJOR);
		*command->version_state = AC_INITRC_VERSION_INVALID;
		return;
	}
	*command->version_state = AC_INITRC_VERSION_EXPECT_MINOR;
}

static void setVersionMinor(const initrc_command_t *command)
{
	if( versionValueMatches(command->tok->tokens[2], AC_INITRC_EXPECTED_VER_MINOR) == false )
	{
		AUTOCODE_MSG_ERROR("unsupported init.rc minor syntax version [%s:%i] %s, expected %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[2],
						   AC_INITRC_EXPECTED_VER_MINOR);
		*command->version_state = AC_INITRC_VERSION_INVALID;
		return;
	}
	*command->version_state = AC_INITRC_VERSION_VALID;
}

void initrcSetVersion(const initrc_command_t *command)
{
	if( command->tok->count != 3 )
	{
		AUTOCODE_MSG_ERROR("setVersion token count [%s:%i] is %i, should be 3",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count);
		*command->version_state = AC_INITRC_VERSION_INVALID;
		return;
	}
	if( (*command->version_state == AC_INITRC_VERSION_EXPECT_MAJOR) &&
		(strcmp(command->tok->tokens[1], "major") != 0) )
	{
		AUTOCODE_MSG_ERROR("first init.rc line [%s:%i] must be setVersion major %i",
						   command->initrc_name,
						   command->file_line_number,
						   AC_INITRC_EXPECTED_VER_MAJOR);
		*command->version_state = AC_INITRC_VERSION_INVALID;
		return;
	}
	if( (*command->version_state == AC_INITRC_VERSION_EXPECT_MINOR) &&
		(strcmp(command->tok->tokens[1], "minor") != 0) )
	{
		AUTOCODE_MSG_ERROR("second init.rc line [%s:%i] must be setVersion minor %i",
						   command->initrc_name,
						   command->file_line_number,
						   AC_INITRC_EXPECTED_VER_MINOR);
		*command->version_state = AC_INITRC_VERSION_INVALID;
		return;
	}
	if( *command->version_state == AC_INITRC_VERSION_VALID )
	{
		AUTOCODE_MSG_ERROR("setVersion command outside header [%s:%i]",
						   command->initrc_name,
						   command->file_line_number);
		*command->version_state = AC_INITRC_VERSION_INVALID;
		return;
	}

	for( size_t i = 0; i < (sizeof(set_version_options) / sizeof(set_version_options[0])); i++ )
	{
		if( strcmp(command->tok->tokens[1], set_version_options[i].name) == 0 )
		{
			(*set_version_options[i].func)(command);
			return;
		}
	}

	AUTOCODE_MSG_ERROR("unknown setVersion option [%s:%i] %s",
					   command->initrc_name,
					   command->file_line_number,
					   command->tok->tokens[1]);
	*command->version_state = AC_INITRC_VERSION_INVALID;
}
