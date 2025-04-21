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
 * - Simple and reliable, read plain text file <tasks_list> and <drivers_list>
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
 * @todo refactoring
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/tokenizer.h"


int main(void)
{
	// allocation
	module_t modules;

	// new tokenizer test
	tokenizer_t data;

	strcpy(data.line, "    hello why ?");
	tokenizer(&data);

	for(int i=0; i<data.tokens_count; i++)
	{
		printf("[autocode.c] token[%i] = <%s>\n",i,data.tokens[i]);
	}


	// read init.rc file and store data in modules
	//listToTable(modules, line, tokens);

	// print found modules
	//printTable(modules);

	// write autoInclude.h files
	//writeInclude(modules);

	// write autoAlloc.h files
	//writeAlloc(modules);

	// read tag to generate code in initSys.c
	//readTag(modules, line, tokens);

	return 0;
}
