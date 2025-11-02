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
 * @file printModules.c
 * @brief print modules content implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode2_src/autoCode.h"
#include "utility/autoCode2_src/printModules.h"

void printModules(const modules_database_t *data_base)
{
	msgInfo("found drivers :");
	const module_type_t *driver = &data_base->modules_type[MODULES_DRIVERS_ID];
	for( int i = 0; i < driver->modules_count; i++ )
	{
		printf("\tdrivers[%i] \"%s\" status=%i \n", i, driver->modules[i].name, driver->modules[i].status);
	}
	printf("\n");

	msgInfo("found services :");
	const module_type_t *services = &data_base->modules_type[MODULES_SERVICES_ID];
	for( int i = 0; i < services->modules_count; i++ )
	{
		printf("\tservices[%i] \"%s\" status=%i\n", i, services->modules[i].name,
			   services->modules[i].status);
	}
	printf("\n");

	msgInfo("found tasks :");
	const module_type_t *tasks = &data_base->modules_type[MODULES_TASKS_ID];
	for( int i = 0; i < tasks->modules_count; i++ )
	{
		printf("\ttasks[%i] \"%s\" status=%i\n", i, tasks->modules[i].name, tasks->modules[i].status);
	}
	printf("\n");

	msgInfo("threads (services + tasks) by run level :");
	char name[256];
	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		switch( i )
		{
			case RUN_NONE:
				strcpy(name, "RUN_NONE");
				break;
			case RUN_CORE:
				strcpy(name, "RUN_CORE");
				break;
			case RUN_DRIVER:
				strcpy(name, "RUN_DRIVER");
				break;
			case RUN_SERVICE:
				strcpy(name, "RUN_SERVICE");
				break;
			case RUN_USER:
				strcpy(name, "RUN_USER");
				break;
			default:
				msgError("unknow run level");
				printf("\t =%i ?\n", i);
				exit(0);
				break;
		}

		printf("\trun_level_threads_count[%s] = %i\n", name,
			   data_base->run_level_module_count[MODULES_SERVICES_ID][i] +
				   data_base->run_level_module_count[MODULES_TASKS_ID][i]);
	}
	printf("\n");
}
