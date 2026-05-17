/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tokenizer.c
 * @brief tokenizer implementation.
 *
 */

#include "tokenizer.h"

void tokenizer(tokenizer_t *tok)
{
	int index_line = 0;
	int token_current;

	// reset tokens
	for( token_current = 0; token_current < TOKEN_COUNT_MAX; token_current++ )
	{
		tok->tokens[token_current][0] = 0;
	}

	// start reading line to extract arguments
	token_current = 0;
	tok->count = 0;

	while( (tok->line[index_line] != '\n') && (tok->line[index_line] != 0) &&
		   (index_line < (TOKEN_LINE_SIZE_MAX - 1)) && (token_current < TOKEN_COUNT_MAX) )
	{
		// get off leading space or tab
		while( ((tok->line[index_line] == ' ') || (tok->line[index_line] == '\t')) &&
			   (index_line < TOKEN_LINE_SIZE_MAX - 1) )
		{
			index_line++;
		}

		// read and store one token
		int index_token = 0;
		char cut_charter = ' ';

		if( tok->line[index_line] == '"' ) // switch to string mode for this token
		{
			cut_charter = '"';
			index_line++;
			tok->tokens[token_current][index_token++] = '"';
		}

		while( (tok->line[index_line] != cut_charter) && (tok->line[index_line] != '\t') &&
			   (tok->line[index_line] != '\n') && (tok->line[index_line] != 0) &&
			   (index_line < (TOKEN_LINE_SIZE_MAX - 1)) && (index_token < (TOKEN_SIZE_MAX - 1)) )
		{
			tok->tokens[token_current][index_token++] = tok->line[index_line++];
		}

		if( tok->line[index_line] == '"' )
		{
			index_line++;
			tok->tokens[token_current][index_token++] = '"';
		}

		tok->tokens[token_current][index_token] = 0;

		if( tok->tokens[token_current][0] != 0 ) { token_current++; }
	}

	tok->count = token_current;
}
