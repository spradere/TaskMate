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
 * @file writeInclude.c
 * @brief write include code implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/writeInclude.h"

void writeInclude(module_t *modules)
{
	// open include files
	FILE *file_task_include = fopen(FILE_TASK_INCLUDE, "w");
	if (file_task_include == NULL)
	{
		msgError("creating file");
		printf("\t <%s>\n", FILE_TASK_INCLUDE);
		exit(1);
	}

	FILE *file_driver_include = fopen(FILE_DRIVER_INCLUDE, "w");
	if (file_driver_include == NULL)
	{
		msgError("creating file");
		printf("\t <%s>\n", FILE_DRIVER_INCLUDE);
		exit(1);
	}

	int i;
	// write task include

	for (i = 0; i < modules->task_count; i++)
	{
		fprintf(file_task_include, "#include \"tasks/%s.h\"\n", modules->tasks[i]->name);
	}

	// write driver include
	for (i = 0; i < modules->driver_count; i++)
	{
		fprintf(file_driver_include, "#include \"drivers/%s.h\"\n", modules->drivers[i]->name);
	}

	fclose(file_task_include);
	fclose(file_driver_include);
}
