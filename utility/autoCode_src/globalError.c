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
typedef enum
{
	ERROR_NOT_DEFINED,
	ERROR_LOW,
	ERROR_MID,
	ERROR_HIGH
}error_critical_t;

typedef struct
{
	char name[256];
	char message[256];
	error_critical_t critical;
} error_item_t;

error_item_t error_catalog[256];


void globalError(const char *file_err_in_name)
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
	int error_count = 0;
	tokenizer_t tok;
	printf("\n");

	strncpy(error_catalog[error_current].name, "ERR_NO_ERROR",256);
	strncpy(error_catalog[error_current].message, "\"No error\"",256);
	error_catalog[error_current].critical = ERROR_LOW;
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

		strncpy(error_catalog[error_current].name, tok.tokens[0],256);
		strncpy(error_catalog[error_current].message, tok.tokens[1],256);

		error_catalog[error_current].critical = ERROR_NOT_DEFINED;
		if(strcmp( tok.tokens[2], "LOW") == 0){error_catalog[error_current].critical = ERROR_LOW;}
		if(strcmp( tok.tokens[2], "MID") == 0){error_catalog[error_current].critical = ERROR_MID;}
		if(strcmp( tok.tokens[2], "HIGH") == 0){error_catalog[error_current].critical = ERROR_HIGH;}
		if( error_catalog[error_current].critical == ERROR_NOT_DEFINED)
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
		error_count = error_current;
	}

	// write errors
	fprintf(file_err_out,"typedef enum\n");
	fprintf(file_err_out,"{\n");

	for(int i=0; i< error_count; i++)
	{
		fprintf(file_err_out,"\t%s,\n",error_catalog[i].name);
	}
	fprintf(file_err_out,"\tERROR_COUNT\n");
	fprintf(file_err_out,"} error_codes_t;\n\n");

	fprintf(file_err_out,"typedef struct\n");
	fprintf(file_err_out,"{\n");
	fprintf(file_err_out,"\tchar *name;\n");
	fprintf(file_err_out,"\terror_critical_t critical;\n");
	fprintf(file_err_out,"} error_item_t;\n\n");

	/*fprintf(file_err_out,"const error_item_t error_catalog[] = \n{\n");

	for(int i=0; i< error_count-1; i++)
	{
		fprintf(file_err_out,"\t{%s, %i},\n", error_catalog[i].message, error_catalog[i].critical);
	}
	fprintf(file_err_out,"\t{%s, %i}\n",
		error_catalog[error_count-1].message, error_catalog[error_count-1].critical);
	fprintf(file_err_out,"};\n");*/

	// end
	fprintf(file_err_out,"\n#endif\n");
	fclose(file_err_in);
	fclose(file_err_out);
}
