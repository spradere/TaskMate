/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file printModules.c
 * @brief print modules implementation.
 *
 */

#include "printModules.h"

void printModules(const modules_database_t *data_base)
{
	msgInfo("+++ modules informations +++\n");

	msgInfo("found drivers :");
	const module_type_t *driver = &data_base->modules_type[TM_MOD_DRIVERS_ID];
	for( int i = 0; i < driver->modules_count; i++ )
	{
		printf("\tdrivers[%i] \"%s\" runlevel=%i\n",
			   i,
			   driver->modules[i].name,
			   driver->modules[i].status);
	}
	printf("\n");

	msgInfo("found threads :");
	const module_type_t *threads = &data_base->modules_type[TM_MOD_THREAD_ID];
	for( int i = 0; i < threads->modules_count; i++ )
	{
		printf("\tthread[%i] \"%s\" runlevel=%i type=%i\n",
			   i,
			   threads->modules[i].name,
			   threads->modules[i].status & RUN_LEVEL_MASK,
			   threads->modules[i].status & (~RUN_LEVEL_MASK));
	}
	printf("\n");

	msgInfo("threads by run level :");
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
				msgError("unknown run level");
				printf("\t =%i ?\n", i);
				exit(1);
				break;
		}

		printf("\trun_level_threads_count[%s] = %i\n", name, data_base->threads_count[i]);
	}
	printf("\n");
}
