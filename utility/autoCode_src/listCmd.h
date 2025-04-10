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
 * @file listCmd.h
 * @brief list command parser header
 *
 * @todo nothing
 */

#ifndef LISTCMD_H
#define LISTCMD_H

#include "utility/autoCode_src/autoCode.h"
extern list_table_t *list_table;

typedef struct
{
	const char *name;
	void (*func)(typeof(list_table->task_list[0]->status) *);

} cmd_t;

int cmdTaskDispatch(const char *cmd, typeof(list_table->task_list[0]->status) *status);
int cmdDriverDispatch(const char *cmd, typeof(list_table->driver_list[0]->status) *status);

#endif

