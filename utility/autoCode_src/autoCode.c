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
 * @file autoCode.c
 * @brief helper to generate code for task and drivers handle in initSys.c
 *
 * - Simple and reliable, read plain text file <task_list> and <driver_list>
 * - Write code for #include / static allocation / initialisation in initSys.c
 *
 * @note
 * tag format are one line C comment // [tag] <task|driver> <init>
 *
 * @warning
 * do not edit code between tag, it will be deleted by automatic generated code !
 * tasks file name must match with function, lcd.c -> void lcd(void),
 * drivers functions name must match to generic driver layout :
 * - <driver name>SetStatus()
 * - <driver name>GetStatus()
 * - <driver name>GetName()
 * - <driver name>Init()
 * - <driver name>Start()
 * - <driver name>Stop()
 *
 * @todo split autoCode.c -> many file/functions, read tag in list file to setup task/driver status
 */
#include <string.h>
#include <assert.h>

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/allocate.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/listToTable.h"
#include "utility/autoCode_src/writeInclude.h"
#include "utility/autoCode_src/writeAlloc.h"
#include "utility/autoCode_src/readTag.h"

int main(void)
{
	// allocation
	list_table_t *list_table = NULL; // task and driver tables
	char *line = NULL; // buffer for reading
	char **argv = NULL; // argument data

	allocate(&list_table, &line, &argv);

	// test allocation

	printf("[%s]\nlist_table %p-> %p\n", __FILE__, &list_table, list_table);
	printf("driver_list %p-> %p\n", (void *)&list_table->driver_list,
		   (void *)list_table->driver_list);
	printf("driver_list[0] %p-> %p\n", (void *)&list_table->driver_list[0],
		   (void *)list_table->driver_list[0]);
	printf("driver_list[0]->name %p-> %p\n", (void *)&list_table->driver_list[0]->name,
		   (void *)list_table->driver_list[0]->name);

	strcpy(list_table->driver_list[0]->name, "hello");
	printf("[%s:%i] name=<%s>\n", __FILE__, __LINE__, list_table->driver_list[0]->name);

	strcpy(line, "one two three");
	int count = tokenizer(line, argv);

	if (count > 0)
	{
		for (int i = 0; i < count; i++)
		{
			printf("[autoCode.c] token[%i]=%s\n", i, argv[i]);
		}
	}
	else
	{
		perror("[autoCode.c] tokenizer fail");
		return (0);
	}

	// read list file and store data in table
	// listToTable(list_table);

	// print tables
	// printTable(list_table);

	// write include files
	// writeInclude(list_table);

	// write alloc files
	// writeAlloc(list_table);

	// read tag to generate code
	// readTag(list_table);

	// free malloc
	unAllocate(&list_table, &line, &argv);

	return 0;
}
