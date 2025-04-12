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
 * @file printTable.c
 * @brief print modules content implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/printTable.h"

void printTable(module_t *modules)
{
	int i;

	printf("[autoCode.c] found task :\n");
	for (i = 0; i < modules->task_count; i++)
	{
		printf("\ttask[%i] name:%s status=%i\n", i, modules->task_list[i]->name,
			   modules->task_list[i]->status);
	}

	printf("\n[autoCode.c] found driver :\n");
	for (i = 0; i < modules->driver_count; i++)
	{
		printf("\tdriver[%i] name:%s status=%i\n", i, modules->driver_list[i]->name,
			   modules->driver_list[i]->status);
	}
	printf("\n");
}
