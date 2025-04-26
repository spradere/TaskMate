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
 * @brief tokenizer implementation, cut one line into tokens
 *
 * @todo Nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/tokenizer.h"

void tokenizer(tokenizer_t *data)
{
	int index_line = 0;
	int index_token = 0;
	int tokens_count;


	// reset all tokens
	for (tokens_count = 0; tokens_count < TOKEN_COUNT_MAX; tokens_count++)
	{
		data->tokens[tokens_count][0] = 0;
	}

	// start read line to extract arguments
	tokens_count = 0;
	data->count = 0;

	while ((data->line[index_line] != '\n') && (data->line[index_line] != 0) &&
		   (index_line < (TOKEN_LINE_SIZE_MAX - 1)) && (tokens_count < TOKEN_COUNT_MAX))
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
			data->tokens[tokens_count][index_token++] = data->line[index_line++];
		}

		data->tokens[tokens_count][index_token] = 0;

		if (data->tokens[tokens_count][0] != 0)
		{
			tokens_count++;
		}
	}

	data->count = tokens_count;
	return;
}
