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

void tokenizer(tokens_t *data)
{
	int index_line = 0;
	int index_token = 0;
	int token_count;

	data->token_count = 0;

	printf("[tokenizer.c] line = <%s>\n",data->line);

	// reset all tokens
	for (token_count = 0; token_count < TOKEN_COUNT_MAX; token_count++)
	{
		data->tokens[token_count][0] = 0;
		printf("[tokenizer.c] token[%i] = <%s>\n",token_count,data->tokens[token_count]);

	}

	// read line, extract arguments
	token_count = 0;

	while ((data->line[index_line] != '\n') && (data->line[index_line] != 0) &&
		   (index_line < (TOKEN_LINE_SIZE_MAX - 1)) && (token_count < TOKEN_COUNT_MAX))
	{
		// get off leading space or tab
		while (((data->line[index_line] == ' ') || (data->line[index_line] == '\t')) &&
			   (index_line < TOKEN_LINE_SIZE_MAX - 1))
		{
			index_line++;
		}

		// read and store token
		index_token = 0;
		while ((data->line[index_line] != ' ') && (data->line[index_line] != '\t') &&
			   (data->line[index_line] != '\n') && (data->line[index_line] != 0) &&
			   (index_line < (TOKEN_LINE_SIZE_MAX - 1)) && (index_token < (TOKEN_SIZE_MAX - 1)))
		{
			data->tokens[token_count][index_token++] = data->line[index_line++];
		}

		data->tokens[token_count][index_token] = 0;
		if (data->tokens[token_count][0] != 0)
		{
			token_count++;
		}
	}

	data->token_count = token_count;
	return;
}
