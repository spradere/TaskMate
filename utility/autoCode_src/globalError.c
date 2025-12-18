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

void globalError(error_catalog_t *errors)
{
	msgInfo("open file.err <%s>", errors->file_src.name);

	// open file
	errors->file_src.stream = fopen(errors->file_src.name, "r");
	if( errors->file_src.stream == NULL )
	{
		msgError("open error file <%s>", errors->file_src.name);
		exit(1);
	}

	errors->file_dest.stream = fopen(errors->file_dest.name, "w");
	if( errors->file_dest.stream == NULL )
	{
		msgError("open error file : %s", errors->file_dest.name);
		exit(1);
	}

	printLicenceHeader(errors->file_dest.stream);
	printWarningHeader(errors->file_dest.stream);

	fprintf(errors->file_dest.stream, "#ifndef ERROR_H\n");
	fprintf(errors->file_dest.stream, "#define ERROR_H\n\n");

	fprintf(errors->file_dest.stream, "typedef enum\n");
	fprintf(errors->file_dest.stream, "{\n");
	fprintf(errors->file_dest.stream, "\tERROR_LOW,\n");
	fprintf(errors->file_dest.stream, "\tERROR_MID,\n");
	fprintf(errors->file_dest.stream, "\tERROR_HIGH\n");
	fprintf(errors->file_dest.stream, "} error_critical_t;\n\n");

	// read form source
	int file_src_line_number = 0;
	int error_index = 0;
	tokenizer_t tok;

	strncpy(errors->catalog[error_index].name, "ERR_NO_ERROR", 256);
	strncpy(errors->catalog[error_index].message, "\"No error\"", 256);
	errors->catalog[error_index].critical = ERROR_LOW;
	error_index++;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, errors->file_src.stream) )
	{

		file_src_line_number++;
		tokenizer(&tok);

		if((tok.tokens[0][0] != '\n') && (tok.tokens[0][0] != '#') && (tok.count != 0))
		{
			if( tok.count != 3 )
			{
				msgError("wrong token count != 3 tok.line [%s:%i] <%s>", errors->file_src.name, file_src_line_number, tok.line);
			}

			// printf("<%s><%s><%s>\n",tok.tokens[0],tok.tokens[1],tok.tokens[2]);

			for( int i = 0; i < error_index; i++ )
			{
				if( strcmp(tok.tokens[0], errors->catalog[i].name) == 0 )
				{
					msgError("Duplicate error name <%s>", tok.tokens[0]);
					exit(1);
				}
			}
			strncpy(errors->catalog[error_index].name, tok.tokens[0], 256);
			strncpy(errors->catalog[error_index].message, tok.tokens[1], 256);

			errors->catalog[error_index].critical = ERROR_NOT_DEFINED;
			if( strcmp(tok.tokens[2], "LOW") == 0 ) { errors->catalog[error_index].critical = ERROR_LOW; }
			if( strcmp(tok.tokens[2], "MID") == 0 ) { errors->catalog[error_index].critical = ERROR_MID; }
			if( strcmp(tok.tokens[2], "HIGH") == 0 ) { errors->catalog[error_index].critical = ERROR_HIGH; }
			if( errors->catalog[error_index].critical == ERROR_NOT_DEFINED )
			{
				msgError("wrong citical argument <%s>", tok.tokens[2]);
				exit(1);
			}

			error_index++;

			if( error_index == 256 )
			{
				msgError("Too many errors > 256");
				exit(1);
			}
			errors->error_count = error_index;
		}
	}

	// write errors enum
	fprintf(errors->file_dest.stream, "typedef enum\n");
	fprintf(errors->file_dest.stream, "{\n");

	for( int i = 0; i < errors->error_count; i++ ) { fprintf(errors->file_dest.stream, "\t%s,\n", errors->catalog[i].name); }
	fprintf(errors->file_dest.stream, "\tERROR_COUNT\n");
	fprintf(errors->file_dest.stream, "} error_codes_t;\n\n");

	fprintf(errors->file_dest.stream, "typedef struct\n");
	fprintf(errors->file_dest.stream, "{\n");
	fprintf(errors->file_dest.stream, "\tchar *name;\n");
	fprintf(errors->file_dest.stream, "\terror_critical_t critical;\n");
	fprintf(errors->file_dest.stream, "} error_item_t;\n\n");

	// end
	fprintf(errors->file_dest.stream, "\n#endif\n");
	fclose(errors->file_src.stream);
	fclose(errors->file_dest.stream);
}
