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
 * @brief print table content implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/printTable.h"

void printTable(list_table_t *table)
{
	int i;

	printf("[autoCode.c] found task :\n");
	for (i = 0; i < table->task_count; i++)
	{
		printf("\ttask[%i]=%s status=%i\n", 
			i, table->task_list[i]->name, table->task_list[i]->status);
	}

	printf("\n[autoCode.c] found driver :\n");
	for (i = 0; i < table->driver_count; i++)
	{
		printf("\tdriver[%i]=%s status=%i\n", 
			i, table->driver_list[i]->name, table->driver_list[i]->status);
	}
	printf("\n");
}
