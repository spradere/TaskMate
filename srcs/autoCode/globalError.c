/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file globalError.c
 * @brief global error implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "globalError.h"

#include "fileUtility.h"
#include "tokenizer.h"

#define GLOBALERROR_COUNT_FIELDS 3U
#define GLOBALERROR_INDEX_LEVEL 2U

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

ac_result_t globalError(const char *src_name, error_catalog_t *errors)
{
	AUTOCODE_MSG_INFO("open file.err <%s>", src_name);

	// Read each catalogue into the shared error table.
	file_t file_src;
	fileInit(&file_src);
	file_src.name = (char *)src_name;
	if( fileOpen(&file_src, "r", FILEUTILITY_MODE_READONLY) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("opening file <%s>", src_name);
		return AC_RESULT_ERROR;
	}

	// Continue numbering after errors from earlier catalogue files.
	int file_src_line_number = 0U;
	int error_index = errors->error_count;
	tokenizer_t tok = {0U};
	char line[TOKENIZER_SIZE_LINEMAX];
	file_get_line_result_t line_result;

	while( (line_result = fileGetLine(&file_src, tok.line, sizeof(tok.line))) ==
		   FILE_GET_LINE_SUCCESS )
	{
		file_src_line_number++;
		// Tokenization changes the line; keep the original for diagnostics.
		snprintf(line, sizeof(line), "%s", tok.line);
		tokenizer_err_t token_error = tokenizer(&tok);
		if( token_error != TOK_ERR_NOERR )
		{
			AUTOCODE_MSG_ERROR("tokenizer [%s:%i]: %s", file_src.name, file_src_line_number,
							   tokenizerErrorMessage(token_error));
			continue;
		}

		if( (tok.count != 0U) && (tok.tokens[0U][0U] != '#') )
		{
			bool error_is_valid = true;

			if( tok.count != GLOBALERROR_COUNT_FIELDS )
			{
				AUTOCODE_MSG_ERROR("wrong token count != %u tok.line [%s:%i] <%s>",
								   GLOBALERROR_COUNT_FIELDS,
								   file_src.name,
								   file_src_line_number,
								   line);

				continue;
			}

			if( error_index >= GLOBALERROR_COUNT_MAX )
			{
				AUTOCODE_MSG_ERROR("Too many errors >= %i", GLOBALERROR_COUNT_MAX);

				continue;
			}

			// Names must be unique across all catalogues already read.
			for( int i = 0U; i < error_index; i++ )
			{
				if( strcmp(tok.tokens[0U], errors->catalog[i].name) == 0U )
				{
					AUTOCODE_MSG_ERROR("Duplicate error name <%s>", tok.tokens[0U]);

					error_is_valid = false;
				}
			}
			if( error_is_valid == false ) { continue; }

			// Check fixed catalogue storage before copying either field.
			const size_t name_length = strlen(tok.tokens[0U]);
			const size_t message_length = strlen(tok.tokens[1U]);
			if( (name_length >= sizeof(errors->catalog[error_index].name) - 1U) ||
				(message_length >= sizeof(errors->catalog[error_index].message) - 1U) )
			{
				AUTOCODE_MSG_ERROR("Error name or message is too long [%s:%i]",
								   file_src.name,
								   file_src_line_number);

				continue;
			}
			memcpy(errors->catalog[error_index].name, tok.tokens[0U], name_length + 1U);
			memcpy(errors->catalog[error_index].message, tok.tokens[1U], message_length + 1U);

			AUTOCODE_MSG_INFO("[%i] %s", error_index, tok.tokens[0U]);

			// Classify the level; FLOW entries carry no message in firmware.
			if( strcmp(tok.tokens[GLOBALERROR_INDEX_LEVEL], "FLOW") == 0U )
			{
				errors->catalog[error_index].level = ERR_LEVEL_FLOW;
				if( strcmp(tok.tokens[1U], "\"\"") != 0U )
				{
					AUTOCODE_MSG_ERROR("FLOW error message must be empty [%s:%i]",
									   file_src.name,
									   file_src_line_number);

					error_is_valid = false;
				}
			}
			else if( strcmp(tok.tokens[GLOBALERROR_INDEX_LEVEL], "WARN") == 0U )
			{
				errors->catalog[error_index].level = ERR_LEVEL_WARN;
			}
			else if( strcmp(tok.tokens[GLOBALERROR_INDEX_LEVEL], "FAIL") == 0U )
			{
				errors->catalog[error_index].level = ERR_LEVEL_FAIL;
			}
			else if( strcmp(tok.tokens[GLOBALERROR_INDEX_LEVEL], "PANIC") == 0U )
			{
				errors->catalog[error_index].level = ERR_LEVEL_PANIC;
			}
			else
			{
				AUTOCODE_MSG_ERROR("wrong error level argument <%s>",
							   tok.tokens[GLOBALERROR_INDEX_LEVEL]);

				error_is_valid = false;
			}
			if( error_is_valid == false ) { continue; }

			// Expose only entries whose name, message, and level passed validation.
			error_index++;

			errors->error_count = error_index;
		}
	}
	if( line_result == FILE_GET_LINE_ERROR )
	{
		AUTOCODE_MSG_ERROR("reading file <%s> after line %i", file_src.name, file_src_line_number);
	}

	// Release per-file parser state even after a read error.
	tokenizerFree(&tok);
	ac_result_t result = (line_result == FILE_GET_LINE_ERROR) ? AC_RESULT_ERROR : AC_RESULT_OK;
	if( fileClose(&file_src) != FILE_UTILITY_OK )
	{
		AUTOCODE_MSG_ERROR("closing file <%s>", src_name);
		result = AC_RESULT_ERROR;
	}
	return result;
}
