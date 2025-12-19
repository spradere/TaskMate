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
 * @file fileUtility.c
 * @brief file utility implementation
 *
 */

#include "utility/autoCode_src/fileUtility.h"

void fileInit(file_t *file)
{
	file->name = NULL;
	file->name_alloc = false;
	file->stream = NULL;
	file->stream_open = false;
}

void fileOpen(file_t *file, const char *mode, const char *caller)
{
	file->stream = fopen(file->name, mode);
	if( file->stream == NULL )
	{
		msgError("from %s opening file <%s>", caller, file->name);
		exit(1);
	}
	file->stream_open = true;
}

void fileMakeTmp(const char *file_src_name, file_t *file_tmp, const char *caller)
{
	file_tmp->name = malloc(strlen(file_src_name) + strlen(".tmp") + 1);
	if( file_tmp->name == NULL )
	{
		msgError("fromm %s malloc <%s>", caller, file_src_name);
		exit(1);
	}
	file_tmp->name_alloc = true;
	sprintf(file_tmp->name, "%s.tmp", file_src_name);
	msgInfo("generated name : <%s> ", file_tmp->name);

	file_tmp->stream = fopen(file_tmp->name, "w");
	if( file_tmp->stream == NULL )
	{
		msgError("from %s creating file <%s>",caller, file_tmp->name);
		exit(1);
	}
	file_tmp->stream_open = true;
}

void printLicenceHeader(FILE *file)
{
	char header_path[] = "templates/licence_header";
	FILE *header_file = fopen(header_path, "r");

	if( header_file == NULL )
	{
		msgError("open error file <%s>", header_path);
		exit(1);
	}

	char c;
	do {
		c = fgetc(header_file);
		if( c != EOF ) { fputc(c, file); }
	} while( c != EOF );

	fputc('\n', file);

	fclose(header_file);
}

void printWarningHeader(FILE *file)
{
	fprintf(file, "// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	fprintf(file, "// ! Auto generated code, do not edit !\n");
	fprintf(file, "// ! any changes will be lost         !\n");
	fprintf(file, "// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
}
