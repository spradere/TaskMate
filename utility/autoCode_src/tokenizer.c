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

int tokenizer(char *line, char **argv)
{
	int index_line = 0;
	int index_arg = 0;
	int argn;

	// reset all argv
	for (argn = 0; argn < ARGN_COUNT_MAX; argn++)
	{
		argv[argn][0] = 0;
	}

	// read line, extract arguments
	argn = 0;

	while ((line[index_line] != '\n') && (line[index_line] != 0) &&
		   (index_line < (LINE_SIZE_MAX - 1)) && (argn < ARGN_COUNT_MAX))
	{
		// get off leading space or tab
		while (((line[index_line] == ' ') || (line[index_line] == '\t')) &&
			   (index_line < LINE_SIZE_MAX - 1))
		{
			index_line++;
		}

		// read and store token
		index_arg = 0;
		while ((line[index_line] != ' ') && (line[index_line] != '\t') &&
			   (line[index_line] != '\n') && (line[index_line] != 0) &&
			   (index_line < (LINE_SIZE_MAX - 1)) && (index_arg < (ARGV_SIZE_MAX - 1)))
		{
			argv[argn][index_arg++] = line[index_line++];
		}

		argv[argn][index_arg] = 0;
		if (argv[argn][0] != 0)
		{
			argn++;
		}
	}

	return argn;
}
