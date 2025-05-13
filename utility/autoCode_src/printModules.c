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

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/printModules.h"

void printModules(const module_t *modules)
{
	msgInfo("found drivers :");
	for( int i = 0; i < modules->drivers_count; i++ )
	{
		printf("\tdrivers[%i] \"%s\" id=%i status=%i \n", i, modules->drivers[i].name, modules->drivers[i].id,
			   modules->drivers[i].status);
	}
	printf("\n");

	msgInfo("found services :");
	for( int i = 0; i < modules->services_count; i++ )
	{
		printf("\tservices[%i] \"%s\" id=%i status=%i\n", i, modules->services[i].name,
			   modules->services[i].id, modules->services[i].status);
	}
	printf("\n");

	msgInfo("found tasks :");
	for( int i = 0; i < modules->tasks_count; i++ )
	{
		printf("\ttasks[%i] \"%s\" id=%i status=%i\n", i, modules->tasks[i].name, modules->tasks[i].id,
			   modules->tasks[i].status);
	}
	printf("\n");

	msgInfo("threads by run level :");
	char name[256];
	for( int i = 0; i < RUN_LEVEL_COUNT; i++ )
	{
		switch( i )
		{
			case 0:
				strcpy(name, "RUN_NONE");
				break;
			case 1:
				strcpy(name, "RUN_CORE");
				break;
			case 2:
				strcpy(name, "RUN_DRIVER");
				break;
			case 3:
				strcpy(name, "RUN_SERVICE");
				break;
			case 4:
				strcpy(name, "RUN_USER");
				break;
		}

		printf("\trun_level_threads_count[%s] = %i\n", name, modules->run_level_threads_count[i]);
	}
	printf("\n");
}
