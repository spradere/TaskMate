/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file writeInclude.c
 * @brief write include code implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/writeInclude.h"

void writeInclude(const module_t *modules, const char *file_name, const char *arch)
{
	FILE *file_include = fopen(file_name, "w");
	if( file_include == NULL )
	{
		msgError("creating file");
		printf("\t <%s>\n", file_name);
		exit(1);
	}

	for( int i = 0; i < modules->drivers_count; i++ )
	{
		fprintf(file_include, "#include \"arch/%s/%s.h\"\n", arch, modules->drivers[i].name);
	}
	fprintf(file_include, "\n");

	for( int i = 0; i < modules->services_count; i++ )
	{
		fprintf(file_include, "#include \"services/%s.h\"\n", modules->services[i].name);
	}
	fprintf(file_include, "\n");

	for( int i = 0; i < modules->tasks_count; i++ )
	{
		fprintf(file_include, "#include \"tasks/%s.h\"\n", modules->tasks[i].name);
	}

	fclose(file_include);
}
