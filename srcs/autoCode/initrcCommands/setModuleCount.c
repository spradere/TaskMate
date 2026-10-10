/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setModuleCount.c
 * @brief init.rc setModuleCount command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "setModuleCount.h"

enum
{
	SETMODULECOUNT_BASE_DECIMAL = 10U,
	SETMODULECOUNT_TOKEN_COUNT = 2U,
	SETMODULECOUNT_INDEX_COUNT = 1U,
	SETMODULECOUNT_WIDTH_8 = 8U,
	SETMODULECOUNT_WIDTH_16 = 16U
};

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void initrcSetModuleCount(const initrc_command_t *command)
{
	if( command->tok->count != SETMODULECOUNT_TOKEN_COUNT )
	{
		AUTOCODE_MSG_ERROR("setModuleCount token count [%s:%i] is %i, should be %i",
						   command->initrc_name,
						   command->file_line_number,
						   command->tok->count,
						   SETMODULECOUNT_TOKEN_COUNT);
		return;
	}

	if( command->data_base->module_count_set )
	{
		AUTOCODE_MSG_ERROR("setModuleCount is already defined [%s:%i]",
						   command->initrc_name,
						   command->file_line_number);
		return;
	}

	const char *count_text = command->tok->tokens[SETMODULECOUNT_INDEX_COUNT];
	char *end;
	const unsigned long count = strtoul(count_text, &end, SETMODULECOUNT_BASE_DECIMAL);
	if( (count_text[0U] == 0U) || (*end != 0U) || (count == 0U) || (count > UINT16_MAX) )
	{
		AUTOCODE_MSG_ERROR("invalid module count [%s:%i] %s, expected 1..%u",
						   command->initrc_name,
						   command->file_line_number,
						   count_text,
						   (unsigned int)UINT16_MAX);
		return;
	}

	command->data_base->module_count_max = (size_t)count;
	command->data_base->module_count_width =
		(count <= UINT8_MAX) ? SETMODULECOUNT_WIDTH_8 : SETMODULECOUNT_WIDTH_16;
	command->data_base->module_count_set = true;
}
