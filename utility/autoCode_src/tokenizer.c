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
 * @todo struct for input/output data
 */

#include "utility/autoCode_src/tokenizer.h"


int tokenizer(char *line, int line_size_max, char **argv, int argn_count_max, int argv_size_max)
{
	int i_line = 0;
	int i_arg = 0;
	int argn;

	// reset all argv
	for (argn = 0; argn < argn_count_max; argn++)
	{
		argv[argn][0] = 0;
	}
	
	// read line, extract arguments
	argn=0;
	
	while (	(line[i_line] != '\n') && (line[i_line] != 0)
			&& (i_line < (line_size_max - 1)) && (argn < argn_count_max)	)	
	{
		// get off leading space or tab
		while (	((line[i_line] == ' ') || (line[i_line] == '\t')) 
				&& (i_line < line_size_max - 1) 	)
		{i_line++;}
		
		// read and store token
		i_arg = 0;
		while (	(line[i_line] != ' ') && (line[i_line] != '\t') 
				&& (line[i_line] != '\n') && (line[i_line] != 0) 
				&& (i_line < (line_size_max - 1)) && (i_arg < (argv_size_max-1))	)
		{argv[argn][i_arg++]=line[i_line++];}

		argv[argn][i_arg] = 0;
		if(argv[argn][0] != 0 ){argn++;}
	}

	return argn;
}


