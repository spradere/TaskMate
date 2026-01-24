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
 * @brief Tool to generate code for threads, drivers and HAL in TaskMate codebase
 *
 * - Simple and reliable, read plain text init.rc files
 * - Write code for #include / static allocation / initialisation via in code
 * tag system
 *
 * @note
 * tag format are one line C comment // [tag] <object> <action>
 *
 */

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * autoCode is a critical component: if it generates incorrect code,
 * TaskMate may still compile but will behave unpredictably at runtime.
 * Any change to autoCode must be considered system-critical and tested
 * accordingly.
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * */

#include "autoCode.h"

#include "fileUtility.h"
#include "globalError.h"
#include "parseInitrc.h"
#include "options.h"
#include "parseTag.h"
#include "printModules.h"
#include "writeInclude.h"

static void setupDB(modules_database_t *data_base);
static void checkModulesCount(modules_database_t *data_base);
static void threadCountLevel(modules_database_t *data_base);

int main(int argn, const char *argv[])
{

	// get options
	if( argn != 2 )
	{
		msgError("autoCode bad argn (is %i, not 2)\n\tuse autoCode config_file", argn);
		exit(1);
	}

	options_list_t auto_options;
	options(argv[1], &auto_options);

	msgInfo("target %s -> %s -> %s", auto_options.arch_name, auto_options.mcu_name, auto_options.board_name);

	// global error system
	error_catalog_t errors_catalog;
	globalError(auto_options.errors_file, &errors_catalog, "src/sysCall/auto_error.h");

	// setup data base
	modules_database_t data_base;
	setupDB(&data_base);

	// read init.rc file and store data in data base
	char arch_initrc_path[BYTE_INDEX];
	char mcu_initrc_path[BYTE_INDEX];
	char board_initrc_path[BYTE_INDEX];

	snprintf(arch_initrc_path, BYTE_INDEX, "src/hal/arch/%s/arch_init.rc", auto_options.arch_name);
	snprintf(mcu_initrc_path, BYTE_INDEX, "src/hal/mcu/%s/mcu_init.rc", auto_options.mcu_name);
	snprintf(board_initrc_path, BYTE_INDEX, "src/hal/board/%s/board_init.rc", auto_options.board_name);

	parseInitrc(MOD_DRIVERS_ID, &data_base, arch_initrc_path);
	parseInitrc(MOD_DRIVERS_ID, &data_base, mcu_initrc_path);
	parseInitrc(MOD_DRIVERS_ID, &data_base, board_initrc_path);
	parseInitrc(MOD_SERVICES_ID, &data_base, "src/services/services_init.rc");
	parseInitrc(MOD_TASKS_ID, &data_base, "src/tasks/tasks_init.rc");

	// check module count autoCode <-> TaskMate
	checkModulesCount(&data_base);

	// count thread for each level
	threadCountLevel(&data_base);

	// parse tag and generate code for init
	parseTag(&data_base, "src/sysCore/runLevel.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/runLevel.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCall/error.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCall/sysCall.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/modules_define.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/modules.c", &errors_catalog, &auto_options);

	// write headers
	writeInclude(&data_base, INCLUDE_THREAD_LIST, "src/sysCore/auto_threads_list.h", &auto_options);
	writeInclude(&data_base, INCLUDE_HAL_SYSTEM_PART, "src/hal/auto_hal_system.h", &auto_options);
	writeInclude(&data_base, INCLUDE_HAL_USER_PART, "src/hal/auto_hal_user.h", &auto_options);
	writeInclude(&data_base, INCLUDE_HAL_DEFINE, "src/hal/auto_hal_define.h", &auto_options);
	writeInclude(&data_base, INCLUDE_HAL_INIT, "src/hal/auto_hal_init.h", &auto_options);

	// print all info about modules
	filePrintModified();
	printModules(&data_base);

	return 0;
}

static void setupDB(modules_database_t *data_base)
{
	for( int i = 0; i < MOD_TYPE_COUNT; i++ )
	{
		for( int j = 0; j < RUN_LEVEL_COUNT; j++ ) { data_base->run_level_module_count[i][j] = 0; }
	}

	data_base->modules_type[MOD_DRIVERS_ID].initrc_arg_count_max = 2;
	data_base->modules_type[MOD_DRIVERS_ID].modules_count = 0;
	data_base->modules_type[MOD_DRIVERS_ID].name = "Drivers";
	data_base->modules_type[MOD_DRIVERS_ID].status_default = RUN_DRIVER;

	data_base->modules_type[MOD_SERVICES_ID].initrc_arg_count_max = 2;
	data_base->modules_type[MOD_SERVICES_ID].modules_count = 0;
	data_base->modules_type[MOD_SERVICES_ID].name = "Services";
	data_base->modules_type[MOD_SERVICES_ID].status_default = RUN_SERVICE;

	data_base->modules_type[MOD_TASKS_ID].initrc_arg_count_max = 2;
	data_base->modules_type[MOD_TASKS_ID].modules_count = 0;
	data_base->modules_type[MOD_TASKS_ID].name = "Task";
	data_base->modules_type[MOD_TASKS_ID].status_default = RUN_USER;
}

static void checkModulesCount(modules_database_t *data_base)
{
	int module_count[MOD_TYPE_COUNT][2];
	int id;

	id = MOD_DRIVERS_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = MOD_DRIVERS_COUNT_MAX;

	id = MOD_SERVICES_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = MOD_SERVICES_COUNT_MAX;

	id = MOD_TASKS_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = MOD_TASKS_COUNT_MAX;

	for( int i = 0; i < MOD_TYPE_COUNT; i++ )
	{
		if( module_count[i][0] > module_count[i][1] )
		{
			msgError("Too many modules ! %s count = %i > TaskMate max %i", data_base->modules_type[i].name,
					 module_count[i][0], module_count[i][1]);
			exit(1);
		}
	}
}

static void threadCountLevel(modules_database_t *data_base)
{
	for( int level = 0; level < RUN_LEVEL_COUNT; level++ )
	{

		// count thread (services + task) for run level
		int thread_count = 0;

		for( int i = 1; i <= level; i++ )
		{
			thread_count += data_base->run_level_module_count[MOD_SERVICES_ID][i];
		}
		for( int i = 1; i <= level; i++ )
		{
			thread_count += data_base->run_level_module_count[MOD_TASKS_ID][i];
		}

		data_base->threads_count[level] = thread_count;
	}
}
