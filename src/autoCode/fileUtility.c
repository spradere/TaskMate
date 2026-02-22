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

#include "fileUtility.h"

#include <ctype.h>

static int file_updated = 0;
static int file_unchanged = 0;

void generateGuardName(const char *file_name, char *guard_name)
{

	int tmp_index = 0;
	int file_index = (int)strlen(file_name) - 1;
	int guard_index = 0;
	char tmp[BYTE_INDEX];

	do {
		tmp[tmp_index++] = file_name[file_index--];
	} while( (file_name[file_index] != '/') && (file_index >= 0) );
	tmp[tmp_index] = 0;

	tmp_index = (int)strlen(tmp) - 1;
	while( tmp_index >= 0 )
	{
		if( tmp[tmp_index] != '_' ) { guard_name[guard_index] = (char)toupper(tmp[tmp_index]); }
		if( (tmp[tmp_index] == '_') || (tmp[tmp_index] == '.') ) { guard_name[guard_index] = '_'; }
		tmp_index--;
		guard_index++;
	}

	guard_name[guard_index] = 0;
}

void filePrintModified(void)
{
	msgInfo("*******************************************************");
	msgInfo(
		"* summary of modified files : %i updated, %i unchanged *", file_updated, file_unchanged);
	msgInfo("*******************************************************");
}

void fileCmpReplace(file_t *file_old, file_t *file_new)
{
	char old[BYTE_INDEX];
	char new[BYTE_INDEX];
	bool same = true;

	if( fseek(file_old->stream, 0L, SEEK_SET) != 0 )
	{
		msgError("fseek file <%s>", file_old->name);
		exit(1);
	}

	if( fseek(file_new->stream, 0L, SEEK_SET) != 0 )
	{
		msgError("fseek file <%s>", file_new->name);
		exit(1);
	}

	while( (feof(file_old->stream) == 0) && (feof(file_new->stream) == 0) )
	{
		fgets(old, sizeof(old), file_old->stream);
		fgets(new, sizeof(new), file_new->stream);
		if( strcmp(old, new) != 0 ) { same = false; }
	}

	if( same == true )
	{
		msgInfo("keep the old one <%s>", file_old->name);
		remove(file_new->name);
		file_unchanged++;
	}
	else
	{
		msgInfo("change for the new one tmp -> <%s>", file_old->name);
		remove(file_old->name);
		rename(file_new->name, file_old->name);
		file_updated++;
	}
}

void fileClose(file_t *file, const char *caller, const int line)
{
	if( file->stream_opened )
	{
		int err = fclose(file->stream);
		if( err != 0 )
		{
			msgError("from [%s:%i] close file <%s>", caller, line, file->name);
			exit(1);
		}
		if( file->name_allocated ) { free(file->name); }
		fileInit(file);
	}
}

void fileInit(file_t *file)
{
	file->name = NULL;
	file->name_allocated = false;
	file->stream = NULL;
	file->stream_opened = false;
}

void fileOpen(file_t *file, const char *mode, const int special_mode, const char *caller,
			  const int line)
{
	if( file->name == NULL )
	{
		msgError("from [%s:%i] NULL name ", caller, line);
		exit(1);
	}

	file->stream = fopen(file->name, mode);
	if( (file->stream == NULL) && (special_mode == FILE_READONLY) )
	{
		msgError("from [%s:%i] opening file <%s>", caller, line, file->name);
		exit(1);
	}

	if( (file->stream == NULL) && (special_mode == FILE_CREATE) && (strcmp(mode, "r") == 0) )
	{
		msgInfo("file don't exist -> creating <%s>", file->name);
		file->stream = fopen(file->name, "w");
		if( file->stream == NULL )
		{
			msgError("from [%s:%i]creating file <%s>", caller, line, file->name);
			exit(1);
		}
		fclose(file->stream);
		file->stream = fopen(file->name, mode);
		if( file->stream == NULL )
		{
			msgError("from [%s:%i]reopening file <%s>", caller, line, file->name);
			exit(1);
		}
	}
	file->stream_opened = true;
}

void fileMakeTmp(const char *file_src_name, file_t *file_tmp, const char *caller, const int line)
{
	file_tmp->name = malloc(strlen(file_src_name) + strlen(".tmp") + 1);
	if( file_tmp->name == NULL )
	{
		msgError("from [%s:%i] malloc <%s>", caller, line, file_src_name);
		exit(1);
	}
	file_tmp->name_allocated = true;
	sprintf(file_tmp->name, "%s.tmp", file_src_name);

	file_tmp->stream = fopen(file_tmp->name, "w+");
	if( file_tmp->stream == NULL )
	{
		msgError("from [%s:%i] creating file <%s>", caller, line, file_tmp->name);
		exit(1);
	}
	file_tmp->stream_opened = true;
}

void printLicenceHeader(FILE *file)
{
	const char header_path[] = "doc/licence_header.txt";
	FILE *header_file = fopen(header_path, "r");

	if( header_file == NULL )
	{
		msgError("open error file <%s>", header_path);
		exit(1);
	}

	int c;
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

void printClangFormatOff(FILE *file) { fprintf(file, "// clang-format off\n"); }

void printClangFormatOn(FILE *file) { fprintf(file, "// clang-format on\n"); }
