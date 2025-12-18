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
