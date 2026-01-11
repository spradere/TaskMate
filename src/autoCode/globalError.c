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

#include "fileUtility.h"
#include "tokenizer.h"

void globalError(const char *src_name, error_catalog_t *errors, const char *dest_name)
{
	msgInfo("open file.err <%s>", src_name);

	// open files
	file_t file_src;
	fileInit(&file_src);
	file_src.name = (char *)src_name;
	fileOpen(&file_src, "r", __FILE__, __LINE__);

	file_t file_dest;
	fileInit(&file_dest);
	file_dest.name = (char *)dest_name;
	fileOpen(&file_dest, "r", __FILE__, __LINE__);

	file_t file_tmp;
	fileInit(&file_tmp);
	fileMakeTmp(dest_name, &file_tmp, __FILE__, __LINE__);

	// write statements
	printLicenceHeader(file_tmp.stream);
	printWarningHeader(file_tmp.stream);

	fprintf(file_tmp.stream, "#ifndef ERROR_H\n");
	fprintf(file_tmp.stream, "#define ERROR_H\n\n");

	fprintf(file_tmp.stream, "typedef enum\n");
	fprintf(file_tmp.stream, "{\n");
	fprintf(file_tmp.stream, "\tERROR_LOW,\n");
	fprintf(file_tmp.stream, "\tERROR_MID,\n");
	fprintf(file_tmp.stream, "\tERROR_HIGH\n");
	fprintf(file_tmp.stream, "} err_critical_t;\n\n");

	// read form source
	int file_src_line_number = 0;
	int error_index = 0;
	tokenizer_t tok;

	strncpy(errors->catalog[error_index].name, "ERR_NO_ERROR", BYTE_INDEX);
	strncpy(errors->catalog[error_index].message, "\"No error\"", BYTE_INDEX);
	errors->catalog[error_index].critical = ERROR_LOW;
	error_index++;

	while( fgets(tok.line, TOKEN_LINE_SIZE_MAX, file_src.stream) )
	{
		file_src_line_number++;
		tokenizer(&tok);

		if( (tok.tokens[0][0] != '\n') && (tok.tokens[0][0] != '#') && (tok.count != 0) )
		{
			if( tok.count != 3 )
			{
				msgError("wrong token count != 3 tok.line [%s:%i] <%s>", file_src.name, file_src_line_number,
						 tok.line);
			}

			for( int i = 0; i < error_index; i++ )
			{
				if( strcmp(tok.tokens[0], errors->catalog[i].name) == 0 )
				{
					msgError("Duplicate error name <%s>", tok.tokens[0]);
					exit(1);
				}
			}
			strncpy(errors->catalog[error_index].name, tok.tokens[0], BYTE_INDEX);
			strncpy(errors->catalog[error_index].message, tok.tokens[1], BYTE_INDEX);

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

			if( error_index == ERROR_COUNT_MAX )
			{
				msgError("Too many errors > %i", ERROR_COUNT_MAX);
				exit(1);
			}
			errors->error_count = error_index;
		}
	}

	// write errors enum
	fprintf(file_tmp.stream, "typedef enum\n");
	fprintf(file_tmp.stream, "{\n");

	for( int i = 0; i < errors->error_count; i++ )
	{
		fprintf(file_tmp.stream, "\t%s,\n", errors->catalog[i].name);
	}
	fprintf(file_tmp.stream, "\tERROR_COUNT\n");
	fprintf(file_tmp.stream, "} err_codes_t;\n\n");

	fprintf(file_tmp.stream, "typedef struct\n");
	fprintf(file_tmp.stream, "{\n");
	fprintf(file_tmp.stream, "\tchar *name;\n");
	fprintf(file_tmp.stream, "\terr_critical_t critical;\n");
	fprintf(file_tmp.stream, "} err_item_t;\n\n");

	// end
	fprintf(file_tmp.stream, "\n#endif\n");

	fileCmpReplace(&file_dest, &file_tmp);

	fileClose(&file_src, __FILE__, __LINE__);
	fileClose(&file_dest, __FILE__, __LINE__);
	fileClose(&file_tmp, __FILE__, __LINE__);
}
