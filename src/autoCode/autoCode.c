/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file autoCode.c
 * @brief autoCode implementation.
 *
 * - Simple and reliable, read plain text init.rc files
 * - Write code for #include / static allocation / initialisation directly in code
 *
 * @note
 * tag format is a one-line C comment: // [autoCode_tag] <object> <action>
 * end tag zone: // [/tag]
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
#include "options.h"
#include "parseInitrc.h"
#include "parseTag.h"
#include "printModules.h"

static void setupDB(modules_database_t *data_base);
static void checkModulesCount(modules_database_t *data_base);
static void threadCountLevel(modules_database_t *data_base);

int main(int argn, const char *argv[])
{
	// get options
	if( argn != 2 )
	{
		msgError("autoCode bad argn (is %i, not 2)\n\tuse autoCode configuration_file", argn);
		exit(1);
	}

	options_list_t auto_options;
	options(argv[1], &auto_options);

	// global error system
	error_catalog_t errors_catalog;
	globalError(auto_options.file_errors_list, &errors_catalog);

	// setup data base
	modules_database_t data_base;
	setupDB(&data_base);

	// read init.rc file and store data in data base
	FILE finitrc = fopen(auto_options.file_initrc_list, "r");
	if(finitrc == NULL) {msgError("File not found : <%s>\n",auto_options.file_initrc_list); exit(1);}
	
	char fname[BYTE_INDEX];
	
	while(fgets(fname, BYTE_INDEX, finitrc)
	{
		parseInitrc(&data_base, fname);
	}
	
	// check module count autoCode <-> TaskMate
	checkModulesCount(&data_base);

	// count thread for each level
	threadCountLevel(&data_base);

	// parse tag and generate code
	parseTag(&data_base, "src/sysCore/runLevel.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/runLevel.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCall/error.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/interfaces/error_catalog.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/hal/public/hal_sysInfo.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/interfaces/modules_define.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/modules.c", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/modules_list.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/hal/public/hal_define.h", &errors_catalog, &auto_options);
	parseTag(&data_base, "src/sysCore/hal_init.h", &errors_catalog, &auto_options);

	// print all info about modules
	printModules(&data_base);
	filePrintModified();
	return 0;
}

static void setupDB(modules_database_t *data_base)
{
	for( int i = 0; i < TM_MOD_TYPE_COUNT; i++ )
	{
		for( int j = 0; j < RUN_LEVEL_COUNT; j++ ) { data_base->run_level_module_count[i][j] = 0; }
	}

	data_base->modules_type[TM_MOD_DRIVERS_ID].initrc_arg_count_max = 2;
	data_base->modules_type[TM_MOD_DRIVERS_ID].modules_count = 0;
	data_base->modules_type[TM_MOD_DRIVERS_ID].name = "Drivers";
	data_base->modules_type[TM_MOD_DRIVERS_ID].status_default = RUN_DRIVER;

	data_base->modules_type[TM_MOD_SERVICES_ID].initrc_arg_count_max = 2;
	data_base->modules_type[TM_MOD_SERVICES_ID].modules_count = 0;
	data_base->modules_type[TM_MOD_SERVICES_ID].name = "Services";
	data_base->modules_type[TM_MOD_SERVICES_ID].status_default = RUN_SERVICE;

	data_base->modules_type[TM_MOD_TASKS_ID].initrc_arg_count_max = 2;
	data_base->modules_type[TM_MOD_TASKS_ID].modules_count = 0;
	data_base->modules_type[TM_MOD_TASKS_ID].name = "Task";
	data_base->modules_type[TM_MOD_TASKS_ID].status_default = RUN_USER;
}

static void checkModulesCount(modules_database_t *data_base)
{
	int module_count[TM_MOD_TYPE_COUNT][2];
	int id;

	id = TM_MOD_DRIVERS_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = TM_MOD_DRIVERS_COUNT_MAX;

	id = TM_MOD_SERVICES_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = TM_MOD_SERVICES_COUNT_MAX;

	id = TM_MOD_TASKS_ID;
	module_count[id][0] = data_base->modules_type[id].modules_count;
	module_count[id][1] = TM_MOD_TASKS_COUNT_MAX;

	for( int i = 0; i < TM_MOD_TYPE_COUNT; i++ )
	{
		if( module_count[i][0] > module_count[i][1] )
		{
			msgError("Too many modules ! %s count = %i > TaskMate max %i",
					 data_base->modules_type[i].name,
					 module_count[i][0],
					 module_count[i][1]);
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
			thread_count += data_base->run_level_module_count[TM_MOD_SERVICES_ID][i];
		}
		for( int i = 1; i <= level; i++ )
		{
			thread_count += data_base->run_level_module_count[TM_MOD_TASKS_ID][i];
		}

		data_base->threads_count[level] = thread_count;
	}
}
