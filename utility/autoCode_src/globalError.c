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


void globalError(const char *file_err_in_name, error_catalog_t *errors)
{
	msgInfo("open error file :");
	printf("\t <%s>\n", file_err_in_name);

	// open file
	FILE *file_err_in = fopen(file_err_in_name, "r");
	if( file_err_in == NULL )
	{
		msgError("open error file :");
		printf("\t <%s>\n", file_err_in_name);
		exit(1);
	}

	FILE *file_err_out = fopen("src/sysCall/error.h", "w");
	if( file_err_out == NULL )
	{
		msgError("open error file : scr/sysCall/error_new.h");
		exit(1);
	}

	printLicenceHeader(file_err_out);

	fprintf(file_err_out, "// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	fprintf(file_err_out, "// Auto generated code, do not edit !\n");
	fprintf(file_err_out, "// any changes will be lost\n");
	fprintf(file_err_out, "// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");

	fprintf(file_err_out,"#ifndef ERROR_H\n");
	fprintf(file_err_out,"#define ERROR_H\n\n");

	fprintf(file_err_out,"typedef enum\n");
	fprintf(file_err_out,"{\n");
	fprintf(file_err_out,"\tERROR_LOW,\n");
	fprintf(file_err_out,"\tERROR_MID,\n");
	fprintf(file_err_out,"\tERROR_HIGH\n");
	fprintf(file_err_out,"} error_critical_t;\n\n");

	// read form source
	int file_line_number = 0;
	int error_current = 0;
	tokenizer_t tok;
	printf("\n");

	strncpy(errors->catalog[error_current].name, "ERR_NO_ERROR",256);
	strncpy(errors->catalog[error_current].message, "\"No error\"",256);
	errors->catalog[error_current].critical = ERROR_LOW;
	error_current++;

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

		for(int i = 0; i < error_current; i++)
		{
			if(strcmp(tok.tokens[0], errors->catalog[i].name) == 0)
			{
				msgError("Duplicate error name :");
				printf("\t<%s>\n", tok.tokens[0]);
				exit(1);
			}
		}
		strncpy(errors->catalog[error_current].name, tok.tokens[0],256);
		strncpy(errors->catalog[error_current].message, tok.tokens[1],256);

		errors->catalog[error_current].critical = ERROR_NOT_DEFINED;
		if(strcmp( tok.tokens[2], "LOW") == 0){errors->catalog[error_current].critical = ERROR_LOW;}
		if(strcmp( tok.tokens[2], "MID") == 0){errors->catalog[error_current].critical = ERROR_MID;}
		if(strcmp( tok.tokens[2], "HIGH") == 0){errors->catalog[error_current].critical = ERROR_HIGH;}
		if( errors->catalog[error_current].critical == ERROR_NOT_DEFINED)
		{
			msgError("wrong citical argument :");
			printf("\t<%s>\n",tok.tokens[2]);
			exit(1);
		}

		error_current++;

		if(error_current == 256)
		{
			msgError("Too many errors > 256");
			exit(1);
		}
		errors->error_count = error_current;
	}

	// write errors enum
	fprintf(file_err_out,"typedef enum\n");
	fprintf(file_err_out,"{\n");

	for(int i=0; i< errors->error_count; i++)
	{
		fprintf(file_err_out,"\t%s,\n",errors->catalog[i].name);
	}
	fprintf(file_err_out,"\tERROR_COUNT\n");
	fprintf(file_err_out,"} error_codes_t;\n\n");

	fprintf(file_err_out,"typedef struct\n");
	fprintf(file_err_out,"{\n");
	fprintf(file_err_out,"\tchar *name;\n");
	fprintf(file_err_out,"\terror_critical_t critical;\n");
	fprintf(file_err_out,"} error_item_t;\n\n");

	// end
	fprintf(file_err_out,"\n#endif\n");
	fclose(file_err_in);
	fclose(file_err_out);
}
