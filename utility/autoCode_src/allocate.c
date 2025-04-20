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
 * @file allocate.c
 * @brief allocation implemetation
 *
 * @todo nothing
 */

#include "utility/autoCode_src/autoCode.h"
#include "utility/autoCode_src/allocate.h"

void allocate(module_t **modules, char **line, char ***tokens)
{
	/*// task and driver modules
	if (((*modules) = malloc(sizeof(*(*modules)))) == NULL)
	{
		msgError("malloc modules fail");
		exit(0);
	}
	if (((*modules)->drivers = malloc(MODULE_DRIVER_COUNT_MAX * sizeof(*(*modules)->drivers))) == NULL)
	{
		msgError("malloc modules->drivers fail");
		exit(0);
	}
	if (((*modules)->tasks = malloc(MODULE_TASK_COUNT_MAX * sizeof(*(*modules)->tasks))) == NULL)
	{
		msgError("malloc modules->tasks fail");
		exit(0);
	}

	for (int i = 0; i < MODULE_DRIVER_COUNT_MAX; i++)
	{
		if (((*modules)->drivers[i] = malloc(sizeof(**(*modules)->drivers))) == NULL)
		{
			msgError("malloc modules->drivers[i] fail");
			exit(0);
		}

		if (((*modules)->drivers[i]->name =
				 malloc(MODULE_NAME_SIZE_MAX * sizeof(*(*modules)->drivers[i]->name))) == NULL)
		{
			msgError("malloc modules->drivers[i]->name fail");
			exit(0);
		}
	}

	for (int i = 0; i < MODULE_TASK_COUNT_MAX; i++)
	{
		if (((*modules)->tasks[i] = malloc(sizeof(**(*modules)->tasks))) == NULL)
		{
			msgError("malloc modules->tasks[i] fail");
			exit(0);
		}

		if (((*modules)->tasks[i]->name =
				 malloc(MODULE_NAME_SIZE_MAX * sizeof(*(*modules)->tasks[i]->name))) == NULL)
		{
			msgError("malloc modules->tasks[i]->name fail");
			exit(0);
		}
	}

	// buffer for reading
	if ((*line = malloc(LINE_SIZE_MAX * sizeof(*(*line)))) == NULL)
	{
		msgError("malloc line fail");
		exit(0);
	}

	// tokens
	if ((*tokens = malloc(TOKEN_COUNT_MAX * sizeof(**tokens))) == NULL)
	{
		msgError("malloc tokens fail");
		exit(0);
	}

	for (int i = 0; i < TOKEN_COUNT_MAX; i++)
	{
		if (((*tokens)[i] = malloc(TOKEN_SIZE_MAX * sizeof(***tokens))) == NULL)
		{
			msgError("malloc tokens[] fail");
			exit(0);
		}
	}*/
}

void unAllocate(module_t **modules, char **line, char ***tokens)
{
	/*// task and driver modules
	for (int i = 0; i < MODULE_DRIVER_COUNT_MAX; i++)
	{
		free((*modules)->drivers[i]->name);
		free((*modules)->drivers[i]);
	}

	for (int i = 0; i < MODULE_TASK_COUNT_MAX; i++)
	{
		free((*modules)->tasks[i]->name);
		free((*modules)->tasks[i]);
	}

	free((*modules)->drivers);
	free((*modules)->tasks);
	free(*modules);

	// buffer for reading
	free(*line);

	// tokens
	for (int i = 0; i < TOKEN_COUNT_MAX; i++)
	{
		free((*tokens)[i]);
	}
	free(**tokens);*/
}
