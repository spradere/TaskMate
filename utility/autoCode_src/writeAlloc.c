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
 * @file writeAlloc.c
 * @brief write alloc code implemetation
 *
 */

#include "writeAlloc.h"
#include "fileUtility.h"

void writeAlloc(const modules_database_t *data_base, const char *file_name)
{
	// open files
	msgInfo("generate allocation tables in <%s> ", file_name);
	file_t file_alloc;
	fileInit(&file_alloc);
	file_alloc.name = (char *)file_name;
	fileOpen(&file_alloc, "r", __FILE__);

	file_t file_tmp;
	fileInit(&file_tmp);
	fileMakeTmp(file_name, &file_tmp, __FILE__);

	// write modules
	printLicenceHeader(file_tmp.stream);
	printWarningHeader(file_tmp.stream);

	fprintf(file_tmp.stream, "#ifndef AUTOALLOC_H\n");
	fprintf(file_tmp.stream, "#define AUTOALLOC_H\n\n");

	fprintf(file_tmp.stream, "#include <stdint.h>\n\n");

	fprintf(file_tmp.stream, "#define DRIVERS_COUNT %i\n",
			data_base->modules_type[MODULES_DRIVERS_ID].modules_count);
	fprintf(file_tmp.stream, "#define THREADS_COUNT %i\n",
			(data_base->modules_type[MODULES_SERVICES_ID].modules_count +
			 data_base->modules_type[MODULES_TASKS_ID].modules_count));

	fprintf(file_tmp.stream, "\n");

	// write run level
	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		fprintf(file_tmp.stream, "#define RUN_LEVEL%i_THREADS_COUNT %i\n", i, data_base->threads_count[i]);
	}

	fprintf(file_tmp.stream, "\ntypedef struct\n");
	fprintf(file_tmp.stream, "{\n");

	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		fprintf(file_tmp.stream, "\tuint16_t level%i[RUN_LEVEL%i_THREADS_COUNT + 1];\n", i, i);
	}

	fprintf(file_tmp.stream, "\tuint16_t *levels[%i];\n", RUN_LEVEL_COUNT);
	fprintf(file_tmp.stream, "\tuint8_t current;\n");
	fprintf(file_tmp.stream, "\tuint8_t next;\n");
	fprintf(file_tmp.stream, "} run_levels_t;\n\n");

	fprintf(file_tmp.stream, "#endif\n");

	fileCmpReplace(&file_alloc, &file_tmp);
	fileClose(&file_alloc, __FILE__);
	fileClose(&file_tmp, __FILE__);
}
