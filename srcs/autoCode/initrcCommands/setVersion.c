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

#define SETVERSION_BASE_DECIMAL 10U
#define SETVERSION_COUNT_TOKENS 3U
#define SETVERSION_INDEX_VALUE 2U

/* -----------------------------------------------
 * Version option dispatch table
 * ---------------------------------------------*/

static bool setVersionMajor(const initrc_command_t *command);
static bool setVersionMinor(const initrc_command_t *command);

#define SET_VERSION_OPTION(X)   \
	X("major", setVersionMajor) \
	X("minor", setVersionMinor)

static const struct
{
	const char *name;
	bool (*func)(const initrc_command_t *command);
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

	// Reject partial numbers and overflow before checking exact compatibility.
	errno = 0U;
	const unsigned long parsed = strtoul(value, &end, SETVERSION_BASE_DECIMAL);
	if( (value[0U] == 0U) || (*end != 0U) || (errno == ERANGE) ) { return false; }
	return parsed == expected;
}

static bool setVersionMajor(const initrc_command_t *command)
{
	if( versionValueMatches(command->tok->tokens[SETVERSION_INDEX_VALUE],
							 AUTOCODE_VERSION_INITRCMAJOR) == false )
	{
		AUTOCODE_MSG_ERROR("unsupported init.rc major syntax version [%s:%i] %s, expected %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[SETVERSION_INDEX_VALUE],
						   AUTOCODE_VERSION_INITRCMAJOR);
		return false;
	}
	return true;
}

static bool setVersionMinor(const initrc_command_t *command)
{
	if( versionValueMatches(command->tok->tokens[SETVERSION_INDEX_VALUE],
							 AUTOCODE_VERSION_INITRCMINOR) == false )
	{
		AUTOCODE_MSG_ERROR("unsupported init.rc minor syntax version [%s:%i] %s, expected %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[SETVERSION_INDEX_VALUE],
						   AUTOCODE_VERSION_INITRCMINOR);
		return false;
	}
	return true;
}

bool initrcSetVersion(const initrc_command_t *command)
{
	if( command->tok->count != SETVERSION_COUNT_TOKENS )
	{
		AUTOCODE_MSG_ERROR("setVersion token count [%s:%i] is %i, should be %u",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count, SETVERSION_COUNT_TOKENS);
		return false;
	}

	for( size_t i = 0U; i < (sizeof(set_version_options) / sizeof(set_version_options[0U])); i++ )
	{
		if( strcmp(command->tok->tokens[1U], set_version_options[i].name) == 0U )
		{
			return (*set_version_options[i].func)(command);
		}
	}

	AUTOCODE_MSG_ERROR("unknown setVersion option [%s:%i] %s",
					   command->initrc_name,
					   command->file_line_number,
					   command->tok->tokens[1U]);
	return false;
}
