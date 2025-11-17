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
 * @todo nothing
 */

#include "utility/autoCode_src/writeInclude.h"

void writeAlloc(const modules_database_t *data_base, const char *file_name)
{
	FILE *file_alloc = fopen(file_name, "w");
	if( file_alloc == NULL )
	{
		msgError("creating file");
		printf("\t <%s>\n", file_name);
		exit(1);
	}

	// write modules
	fprintf(file_alloc, "// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	fprintf(file_alloc, "// Auto generated code, do not edit !\n");
	fprintf(file_alloc, "// any changes will be lost\n\n");
	fprintf(file_alloc, "// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");

	fprintf(file_alloc, "#include \"sysCore/modules_items.h\"\n");
	fprintf(file_alloc, "\n");

	fprintf(file_alloc, "#define DRIVERS_COUNT %i\n",
			data_base->modules_type[MODULES_DRIVERS_ID].modules_count);
	fprintf(file_alloc, "#define THREADS_COUNT %i\n",
			(data_base->modules_type[MODULES_SERVICES_ID].modules_count +
			 data_base->modules_type[MODULES_TASKS_ID].modules_count));

	fprintf(file_alloc, "\n");

	fprintf(file_alloc, "typedef struct\n");
	fprintf(file_alloc, "{\n");
	fprintf(file_alloc, "\tdriver_item_t drivers[DRIVERS_COUNT];\n");
	fprintf(file_alloc, "\tthread_item_t threads[THREADS_COUNT];\n");
	fprintf(file_alloc, "\tuint8_t thread_current;\n");
	fprintf(file_alloc, "} modules_t;\n\n");

	// write run level
	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		fprintf(file_alloc, "#define RUN_LEVEL%i_THREADS_COUNT %i\n", i, data_base->threads_count[i]);
	}

	fprintf(file_alloc, "\ntypedef struct\n");
	fprintf(file_alloc, "{\n");

	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		fprintf(file_alloc, "\tuint16_t level%i[RUN_LEVEL%i_THREADS_COUNT + 1];\n", i, i);
	}

	fprintf(file_alloc, "\tuint16_t *levels[%i];\n", RUN_LEVEL_COUNT);
	fprintf(file_alloc, "\tuint8_t current;\n");
	fprintf(file_alloc, "\tuint8_t next;\n");
	fprintf(file_alloc, "} run_levels_t;\n\n");

	fclose(file_alloc);
}
