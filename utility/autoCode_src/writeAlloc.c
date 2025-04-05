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
 * @file writeAlloc.c
 * @brief write alloc code implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/writeInclude.h"

void writeAlloc(list_table_t *table)
{
	/*// open include files
	FILE *file_task_alloc = fopen(FILE_TASK_ALLOC, "w");
	if (file_task_alloc == NULL)
	{
		ERRMSG("creating file");
		printf("\t <%s>\n", FILE_TASK_ALLOC);
		exit(1);
	}

	FILE *file_driver_alloc = fopen(FILE_DRIVER_ALLOC, "w");
	if (file_driver_alloc == NULL)
	{
		ERRMSG("creating file");
		printf("\t  <%s>\n", FILE_DRIVER_ALLOC);
		exit(1);
	}

	// write task static alloc
	fprintf(file_task_alloc, "const uint8_t TASK_COUNT = %i;\n", task_count);
	fprintf(file_task_alloc, "task_table_t task_table[%i];\n", task_count);
	fprintf(file_task_alloc, "uint8_t task_current = 0;\n");

	// write driver static alloc
	fprintf(file_driver_alloc, "const uint8_t DRIVER_COUNT = %i;\n", driver_count);
	fprintf(file_driver_alloc, "driver_table_t driver_table[%i];\n", driver_count);

	fclose(file_task_alloc);
	fclose(file_driver_alloc);*/
}
