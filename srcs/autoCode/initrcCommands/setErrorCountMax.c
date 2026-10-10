/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setErrorCountMax.c
 * @brief init.rc setErrorCountMax command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "setErrorCountMax.h"

#include <errno.h>

#include "../globalError.h"

enum
{
	SETERRORCOUNTMAX_BASE_DECIMAL = 10U,
	SETERRORCOUNTMAX_TOKEN_COUNT = 2U,
	SETERRORCOUNTMAX_INDEX_COUNT = 1U,
	SETERRORCOUNTMAX_WIDTH_8 = 8U,
	SETERRORCOUNTMAX_WIDTH_16 = 16U
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void initrcSetErrorCountMax(const initrc_command_t *command)
{
	if( command->tok->count != SETERRORCOUNTMAX_TOKEN_COUNT )
	{
		AUTOCODE_MSG_ERROR("setErrorCountMax token count [%s:%i] is %i, should be %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count,
						   SETERRORCOUNTMAX_TOKEN_COUNT);
		return;
	}

	if( command->data_base->error_count_set )
	{
		AUTOCODE_MSG_ERROR("setErrorCountMax is already defined [%s:%i]",
						   command->initrc_name,
						   command->file_line_number);
		return;
	}

	const char *count_text = command->tok->tokens[SETERRORCOUNTMAX_INDEX_COUNT];
	char *end;
	errno = 0U;
	const unsigned long count = strtoul(count_text, &end, SETERRORCOUNTMAX_BASE_DECIMAL);
	if( (count_text[0U] == 0U) || (*end != 0U) || (errno == ERANGE) || (count == 0U) ||
		(count > GLOBALERROR_COUNT_MAX) )
	{
		AUTOCODE_MSG_ERROR("invalid error count [%s:%i] %s, expected 1..%u",
						   command->initrc_name,
						   command->file_line_number,
						   count_text,
						   (unsigned int)GLOBALERROR_COUNT_MAX);
		return;
	}

	if( command->data_base->error_count > count )
	{
		AUTOCODE_MSG_ERROR("error count exceeds configured maximum %lu [%s:%i]",
						   count,
						   command->initrc_name,
						   command->file_line_number);
		return;
	}

	command->data_base->error_count_max = (size_t)count;
	command->data_base->error_count_width =
		(count <= UINT8_MAX) ? SETERRORCOUNTMAX_WIDTH_8 : SETERRORCOUNTMAX_WIDTH_16;
	command->data_base->error_count_set = true;
}
