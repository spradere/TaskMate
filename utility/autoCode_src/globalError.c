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
 * @file global_error.c
 * @brief write all error in one file
 *
 */

#include "globalError.h"
#include "tokenizer.h"

void globalError(const char *file_err_in_name)
{
	msgInfo("open error file :");
	printf("\t <%s>\n\n", file_err_in_name);

	// open file
	FILE *file_err_in = fopen(file_err_in_name, "r");
	if( file_err_in == NULL )
	{
		msgError("open error file :");
		printf("\t <%s>\n", file_err_in_name);
		exit(1);
	}

	FILE *file_err_out = fopen("src/sysCall/error_new.h", "w");
	if( file_err_out == NULL )
	{
		msgError("open error file : scr/sysCall/error_new.h");
		exit(1);
	}

	printLicenceHeader(file_err_out);

	// todo
	// set up error structure
	// read input file -> structure
	// read structure -> output file

	// read form source
	int file_line_number = 0;
	tokenizer_t tok;
	printf("\n");

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_err_in) )
	{
		file_line_number++;
		tokenizer(&tok);
		if( tok.count != 3 )
		{
			msgError("wrong token count != 3 tok.line :");
			printf("\t [%s:%i] %s\n\n", file_err_in_name, file_line_number, tok.line);
		}

		printf("<%s><%s><%s>\n",tok.tokens[0],tok.tokens[1],tok.tokens[2]);
	}

	fclose(file_err_in);
	fclose(file_err_out);
}
