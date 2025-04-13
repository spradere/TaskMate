/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file tokenizer.c
 * @brief tokenizer implementation, cut one line to tokens
 *
 * @todo Nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/tokenizer.h"

int tokenizer(const char *line, char **tokens)
{
	int index_line = 0;
	int index_token = 0;
	int token_count;

	// reset all tokens
	for (token_count = 0; token_count < TOKEN_COUNT_MAX; token_count++)
	{
		tokens[token_count][0] = 0;
	}

	// read line, extract arguments
	token_count = 0;

	while ((line[index_line] != '\n') && (line[index_line] != 0) &&
		   (index_line < (LINE_SIZE_MAX - 1)) && (token_count < TOKEN_COUNT_MAX))
	{
		// get off leading space or tab
		while (((line[index_line] == ' ') || (line[index_line] == '\t')) &&
			   (index_line < LINE_SIZE_MAX - 1))
		{
			index_line++;
		}

		// read and store token
		index_token = 0;
		while ((line[index_line] != ' ') && (line[index_line] != '\t') &&
			   (line[index_line] != '\n') && (line[index_line] != 0) &&
			   (index_line < (LINE_SIZE_MAX - 1)) && (index_token < (TOKEN_SIZE_MAX - 1)))
		{
			tokens[token_count][index_token++] = line[index_line++];
		}

		tokens[token_count][index_token] = 0;
		if (tokens[token_count][0] != 0)
		{
			token_count++;
		}
	}

	return token_count;
}
