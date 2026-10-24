/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setInitrcVersion.c
 * @brief init.rc setInitrcVersion command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "setInitrcVersion.h"

#include <errno.h>

#define SETINITRCVERSION_BASE_DECIMAL 10U
#define SETINITRCVERSION_COUNT_TOKENS 3U
#define SETINITRCVERSION_INDEX_VALUE 2U

/* -----------------------------------------------
 * Version option dispatch table
 * ---------------------------------------------*/

static bool setInitrcVersionMajor(const initrc_command_t *command);
static bool setInitrcVersionMinor(const initrc_command_t *command);

#define SET_INITRC_VERSION_OPTION(X)   \
	X("major", setInitrcVersionMajor) \
	X("minor", setInitrcVersionMinor)

static const struct
{
	const char *name;
	bool (*func)(const initrc_command_t *command);
} set_initrc_version_options[] = {
#define X(name, func) {(name), (func)},
	SET_INITRC_VERSION_OPTION(X)
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
	const unsigned long parsed = strtoul(value, &end, SETINITRCVERSION_BASE_DECIMAL);
	if( (value[0U] == 0U) || (*end != 0U) || (errno == ERANGE) ) { return false; }
	return parsed == expected;
}

static bool setInitrcVersionMajor(const initrc_command_t *command)
{
	if( versionValueMatches(command->tok->tokens[SETINITRCVERSION_INDEX_VALUE],
							AUTOCODE_VERSION_INITRCMAJOR) == false )
	{
		AUTOCODE_MSG_ERROR("unsupported init.rc major syntax version [%s:%i] %s, expected %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[SETINITRCVERSION_INDEX_VALUE],
						   AUTOCODE_VERSION_INITRCMAJOR);
		return false;
	}
	return true;
}

static bool setInitrcVersionMinor(const initrc_command_t *command)
{
	if( versionValueMatches(command->tok->tokens[SETINITRCVERSION_INDEX_VALUE],
							AUTOCODE_VERSION_INITRCMINOR) == false )
	{
		AUTOCODE_MSG_ERROR("unsupported init.rc minor syntax version [%s:%i] %s, expected %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->tokens[SETINITRCVERSION_INDEX_VALUE],
						   AUTOCODE_VERSION_INITRCMINOR);
		return false;
	}
	return true;
}

bool initrcSetInitrcVersion(const initrc_command_t *command)
{
	if( command->tok->count != SETINITRCVERSION_COUNT_TOKENS )
	{
		AUTOCODE_MSG_ERROR("setInitrcVersion token count [%s:%i] is %i, should be %u",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count,
						   SETINITRCVERSION_COUNT_TOKENS);
		return false;
	}

	for( size_t i = 0U;
		 i < (sizeof(set_initrc_version_options) / sizeof(set_initrc_version_options[0U])); i++ )
	{
		if( strcmp(command->tok->tokens[1U], set_initrc_version_options[i].name) == 0U )
		{
			return (*set_initrc_version_options[i].func)(command);
		}
	}

	AUTOCODE_MSG_ERROR("unknown setInitrcVersion option [%s:%i] %s",
					   command->initrc_name,
					   command->file_line_number,
					   command->tok->tokens[1U]);
	return false;
}
