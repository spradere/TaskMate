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
 * @todo concat 3 parseInitrc in 1
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/parseInitrc.h"
#include "utility/autoCode_src/printdata_base.h"
#include "utility/autoCode_src/writeInclude.h"
#include "utility/autoCode_src/writeAlloc.h"
#include "utility/autoCode_src/parseTag.h"

int main(int argn, char *argv[])
{

	// test command line arguments
	if(argn != 2 ){msgError("Bad argn for autoCode, forget arch ?"); exit(0);}
	char *arch_name = argv[1];
	msgInfo("arch_name");
	printf("\t <%s>\n\n",arch_name);

	// setup data_base
	data_base_t data_base;

	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		data_base.run_level_threads_count[i] = 0;
		data_base.run_level_drivers_count[i] = 0;
	}

	// read init.rc file and store data in data_base[]
	char arch_initrc_path[256];
	sprintf(arch_initrc_path,"src/arch/%s/drivers_init.rc",arch_name);

	parseInitrc(INITRC_DRIVERS, &data_base.modules[MODULE_DRIVERS], arch_initrc_path);
	parseInitrc(INITRC_SERVICES, &data_base.modules[MODULE_SERVICES], "src/services/services_init.rc");
	parseInitrc(INITRC_TASK, &data_base.modules[MODULE_TASK], "src/tasks/tasks_init.rc");

	//
	// add module count check autoCode <-> Taskmate
	//

	// parse tag and generate code for init
	parseTag(&data_base, "src/sysCore/initSys.c");

	// write headers
	writeInclude(&data_base, "src/sysCore/autoInclude.h", arch_name);
	writeAlloc(&data_base, "src/sysCore/autoAlloc.h");

	// print all info about data_base
	printdata_base(&data_base);

	return 0;
}
