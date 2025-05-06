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
 * @file autoCode.c
 * @brief helper to generate code for threads and drivers handle in initSys.c
 *
 * - Simple and reliable, read plain text init.rc file for drivers, system
 * services and user tasks
 * - Write code for #include / static allocation / initialisation in initSys.c
 *
 * @note
 * tag format are one line C comment // [tag] <task|driver> <init>
 *
 * @warning
 * do not edit code between tag, it will be deleted by automatic generated code
 * ! tasks file name must match with function, lcd.c -> void lcd(void), drivers
 * functions name must match to generic driver layout :
 * - <driver name>SetStatus()
 * - <driver name>GetStatus()
 * - <driver name>GetName()
 * - <driver name>Init()
 * - <driver name>Start()
 * - <driver name>Stop()
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/parseInitrc.h"
#include "utility/autoCode_src/printModules.h"
#include "utility/autoCode_src/writeInclude.h"
#include "utility/autoCode_src/writeAlloc.h"
#include "utility/autoCode_src/parseTag.h"

int main(void)
{
	module_t modules;

	for( int i = 0; i < RUN_LEVEL_COUNT; i++ ) { modules.run_level_modules_count[i] = 0; }

	// read init.rc file and store data in modules[]
	parseInitrcDrivers(&modules, "src/drivers/drivers_init.rc");
	parseInitrcServices(&modules, "src/services/services_init.rc");
	parseInitrcTasks(&modules, "src/tasks/tasks_init.rc");

	// write headers
	writeInclude(&modules, "src/sysCore/autoInclude.h");
	writeAlloc(&modules, "src/sysCore/autoAlloc.h");

	// parse tag and generate code for init
	parseTag(&modules, "src/sysCore/initSys.c");

	// print all info about modules
	printModules(&modules);

	return 0;
}
