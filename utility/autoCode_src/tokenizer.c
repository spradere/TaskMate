/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file tokenizer.c
 * @brief tokenizer implementation, cut one line into tokens
 *
 */

#include "utility/autoCode_src/tokenizer.h"

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
		while( (tok->line[index_line] != ' ') && (tok->line[index_line] != '\t') &&
			   (tok->line[index_line] != '\n') && (tok->line[index_line] != 0) &&
			   (index_line < (TOKEN_LINE_SIZE_MAX - 1)) && (index_token < (TOKEN_SIZE_MAX - 1)) )
		{
			tok->tokens[token_current][index_token++] = tok->line[index_line++];
		}

		tok->tokens[token_current][index_token] = 0;

		if( tok->tokens[token_current][0] != 0 ) { token_current++; }
	}

	tok->count = token_current;
}
