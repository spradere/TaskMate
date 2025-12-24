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
 */

#include "printModules.h"

void printModules(const modules_database_t *data_base)
{
	msgInfo("found drivers :");
	const module_type_t *driver = &data_base->modules_type[MODULES_DRIVERS_ID];
	for( int i = 0; i < driver->modules_count; i++ )
	{
		printf("\tdrivers[%i] \"%s\" status=%i\n", i, driver->modules[i].name, driver->modules[i].status);
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
	char name[BYTE_INDEX];
	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		switch( i )
		{
			case RUN_NONE:
				strncpy(name, "RUN_NONE", sizeof(name));
				break;
			case RUN_CORE:
				strncpy(name, "RUN_CORE", sizeof(name));
				break;
			case RUN_DRIVER:
				strncpy(name, "RUN_DRIVER", sizeof(name));
				break;
			case RUN_SERVICE:
				strncpy(name, "RUN_SERVICE", sizeof(name));
				break;
			case RUN_USER:
				strncpy(name, "RUN_USER", sizeof(name));
				break;
			default:
				msgError("unknow run level");
				printf("\t =%i ?\n", i);
				exit(1);
				break;
		}

		printf("\trun_level_threads_count[%s] = %i\n", name, data_base->threads_count[i]);
	}
	printf("\n");
}
