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
 * @todo exit properly
 */

#include <string.h>
#include <assert.h>

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/allocate.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/listToTable.h"
#include "utility/autoCode_src/printTable.h"
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

	// read list file and store data in table
	listToTable(list_table, line, argv);

	// print tables
	printTable(list_table);

	// write include files
	writeInclude(list_table);

	// write alloc files
	writeAlloc(list_table);

	// read tag to generate code
	readTag(list_table, line, argv);

	// free malloc
	unAllocate(&list_table, &line, &argv);

	return 0;
}
