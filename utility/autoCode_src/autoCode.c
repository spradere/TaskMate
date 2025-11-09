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
 * @todo add dry run
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * autoCode is a critical component: if it generates incorrect code,
 * TaskMate may still compile but will behave unpredictably at runtime.
 * Any change to autoCode must be considered system-critical and tested accordingly.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/tokenizer.h"
#include "utility/autoCode_src/parseInitrc.h"
#include "utility/autoCode_src/printModules.h"
#include "utility/autoCode_src/parseTag.h"
#include "utility/autoCode_src/writeInclude.h"
#include "utility/autoCode_src/writeAlloc.h"

static void setupDB(modules_database_t *data_base);
static void checkModulesCount(modules_database_t *data_base);

int main(int argn, char *argv[])
{

	// test command line arguments
	if( argn != 4 )
	{
		msgError("Bad argn for autoCode, forget arch / mcu / board ?");
		exit(0);
	}
	const char *arch_name = argv[1];
	const char *mcu_name = argv[2];
	const char *board_name = argv[3];

	msgInfo("target : ");
	printf("\t %s/%s/%s \n\n", arch_name, mcu_name, board_name);

	// setup data_base
	modules_database_t data_base;
	setupDB(&data_base);

	// read init.rc file and store data in data_base[]
	char arch_initrc_path[256];
	char mcu_initrc_path[256];
	char board_initrc_path[256];

	sprintf(arch_initrc_path, "src/arch/%s/drivers_init.rc", arch_name);
	sprintf(mcu_initrc_path, "src/arch/%s/%s/drivers_init.rc", arch_name, mcu_name);
	sprintf(board_initrc_path, "src/arch/%s/%s/%s/drivers_init.rc", arch_name, mcu_name, board_name);

	parseInitrc(MODULES_DRIVERS_ID, &data_base, arch_initrc_path);
	parseInitrc(MODULES_DRIVERS_ID, &data_base, mcu_initrc_path);
	parseInitrc(MODULES_DRIVERS_ID, &data_base, board_initrc_path);
	parseInitrc(MODULES_SERVICES_ID, &data_base, "src/services/services_init.rc");
	parseInitrc(MODULES_TASKS_ID, &data_base, "src/tasks/tasks_init.rc");

	// check module count autoCode <-> TaskMate
	checkModulesCount(&data_base);

	// parse tag and generate code for init
	parseTag(&data_base, "src/sysCore/initSys.c");
	parseTag(&data_base, "src/sysCore/runLevel.c");

	// write headers
	writeInclude(&data_base, "src/sysCore/autoInclude.h", arch_name, mcu_name, board_name);
	writeAlloc(&data_base, "src/sysCore/autoAlloc.h");

	// print all info about modules
	printModules(&data_base);

	return 0;
}

static void setupDB(modules_database_t *data_base)
{
	for( int i = 0; i < MODULES_TYPE_COUNT; i++ )
	{
		for( int j = 0; j < RUN_LEVEL_COUNT; j++ ) { data_base->run_level_module_count[i][j] = 0; }
	}

	data_base->modules_type[MODULES_DRIVERS_ID].initrc_arg_count_max = 2;
	data_base->modules_type[MODULES_DRIVERS_ID].modules_count = 0;
	data_base->modules_type[MODULES_DRIVERS_ID].name = "Drivers";
	data_base->modules_type[MODULES_DRIVERS_ID].status_default = RUN_DRIVER;

	data_base->modules_type[MODULES_SERVICES_ID].initrc_arg_count_max = 2;
	data_base->modules_type[MODULES_SERVICES_ID].modules_count = 0;
	data_base->modules_type[MODULES_SERVICES_ID].name = "Sevices";
	data_base->modules_type[MODULES_SERVICES_ID].status_default = RUN_SERVICE;

	data_base->modules_type[MODULES_TASKS_ID].initrc_arg_count_max = 2;
	data_base->modules_type[MODULES_TASKS_ID].modules_count = 0;
	data_base->modules_type[MODULES_TASKS_ID].name = "Task";
	data_base->modules_type[MODULES_TASKS_ID].status_default = RUN_USER;
}

static void checkModulesCount(modules_database_t *data_base)
{
	int module_count[MODULES_TYPE_COUNT][2];
	int id;

	id = MODULES_DRIVERS_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = MODULES_DRIVERS_COUNT_MAX;

	id = MODULES_SERVICES_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = MODULES_SERVICES_COUNT_MAX;

	id = MODULES_TASKS_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = MODULES_TASKS_COUNT_MAX;

	for( int i = 0; i < MODULES_TYPE_COUNT; i++ )
	{
		if( module_count[i][0] > module_count[i][1] )
		{
			msgError("Too many modules !");
			printf("\t %s count = %i > TaskMate max %i\n\n", data_base->modules_type[i].name,
				   module_count[i][0], module_count[i][1]);
			exit(0);
		}
	}
}
