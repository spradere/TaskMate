/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setStackSizeMin.c
 * @brief init.rc setStackSizeMin command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "setStackSizeMin.h"

#include <errno.h>

enum
{
	SETSTACKSIZEMIN_BASE_DECIMAL = 10U,
	SETSTACKSIZEMIN_TOKEN_COUNT = 2U,
	SETSTACKSIZEMIN_INDEX_SIZE = 1U
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void initrcSetStackSizeMin(const initrc_command_t *command)
{
	if( command->tok->count != SETSTACKSIZEMIN_TOKEN_COUNT )
	{
		AUTOCODE_MSG_ERROR("setStackSizeMin token count [%s:%i] is %i, should be %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count,
						   SETSTACKSIZEMIN_TOKEN_COUNT);
		return;
	}

	if( command->data_base->stack_size_min_set )
	{
		AUTOCODE_MSG_ERROR("setStackSizeMin is already defined [%s:%i]",
						   command->initrc_name,
						   command->file_line_number);
		return;
	}

	if( command->data_base->module_count > 0U )
	{
		AUTOCODE_MSG_ERROR("setStackSizeMin must be defined before addModule [%s:%i]",
						   command->initrc_name,
						   command->file_line_number);
		return;
	}

	const char *size_text = command->tok->tokens[SETSTACKSIZEMIN_INDEX_SIZE];
	char *end;
	errno = 0U;
	const unsigned long size = strtoul(size_text, &end, SETSTACKSIZEMIN_BASE_DECIMAL);
	if( (size_text[0U] == 0U) || (*end != 0U) || (errno == ERANGE) ||
		(size < AUTOCODE_SIZE_STACKMIN) || (size > UINT16_MAX) )
	{
		AUTOCODE_MSG_ERROR("invalid minimum stack size [%s:%i] %s, expected %lu..%u",
						   command->initrc_name,
						   command->file_line_number,
						   size_text,
						   AUTOCODE_SIZE_STACKMIN,
						   (unsigned int)UINT16_MAX);
		return;
	}

	command->data_base->stack_size_min = (uint16_t)size;
	command->data_base->stack_size_min_set = true;
}
