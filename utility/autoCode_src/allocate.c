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
	// task and driver modules
	if (((*modules) = malloc(sizeof(*(*modules)))) == NULL)
	{
		ERRMSG("malloc modules");
		exit(0);
	}
	if (((*modules)->driver_list = malloc(DRIVER_COUNT_MAX * sizeof(*(*modules)->driver_list))) == NULL)
	{
		ERRMSG("malloc modules->driver_list");
		exit(0);
	}
	if (((*modules)->task_list = malloc(TASK_COUNT_MAX * sizeof(*(*modules)->task_list))) == NULL)
	{
		ERRMSG("malloc modules->task_list");
		exit(0);
	}

	for (int i = 0; i < DRIVER_COUNT_MAX; i++)
	{
		if (((*modules)->driver_list[i] = malloc(sizeof(**(*modules)->driver_list))) == NULL)
		{
			ERRMSG("malloc modules->driver_list[i]");
			exit(0);
		}

		if (((*modules)->driver_list[i]->name =
				 malloc(NAME_SIZE_MAX * sizeof(*(*modules)->driver_list[i]->name))) == NULL)
		{
			ERRMSG("malloc modules->driver_list[i]->name");
			exit(0);
		}
	}

	for (int i = 0; i < TASK_COUNT_MAX; i++)
	{
		if (((*modules)->task_list[i] = malloc(sizeof(**(*modules)->task_list))) == NULL)
		{
			ERRMSG("malloc modules->task_list[i]");
			exit(0);
		}

		if (((*modules)->task_list[i]->name =
				 malloc(NAME_SIZE_MAX * sizeof(*(*modules)->task_list[i]->name))) == NULL)
		{
			ERRMSG("malloc modules->task_list[i]->name");
			exit(0);
		}
	}

	// buffer for reading
	if ((*line = malloc(LINE_SIZE_MAX * sizeof(*(*line)))) == NULL)
	{
		ERRMSG("malloc line");
		exit(0);
	}

	// tokens
	if ((*tokens = malloc(TOKEN_COUNT_MAX * sizeof(**tokens))) == NULL)
	{
		ERRMSG("malloc tokens\n");
		exit(0);
	}

	for (int i = 0; i < TOKEN_COUNT_MAX; i++)
	{
		if (((*tokens)[i] = malloc(TOKEN_SIZE_MAX * sizeof(***tokens))) == NULL)
		{
			ERRMSG("malloc tokens[]\n");
			exit(0);
		}
	}
}

void unAllocate(module_t **modules, char **line, char ***tokens)
{
	// task and driver modules
	for (int i = 0; i < DRIVER_COUNT_MAX; i++)
	{
		free((*modules)->driver_list[i]->name);
		free((*modules)->driver_list[i]);
	}

	for (int i = 0; i < TASK_COUNT_MAX; i++)
	{
		free((*modules)->task_list[i]->name);
		free((*modules)->task_list[i]);
	}

	free((*modules)->driver_list);
	free((*modules)->task_list);
	free(*modules);

	// buffer for reading
	free(*line);

	// tokens
	for (int i = 0; i < TOKEN_COUNT_MAX; i++)
	{
		free((*tokens)[i]);
	}
	free(*tokens);
}
