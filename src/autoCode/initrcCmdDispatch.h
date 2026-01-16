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
 * @file initrcCmdDispatch.h
 * @brief initrc dispatcher header
 *
 */

#ifndef INITRC_CMD_DISPATCH_H
#define INITRC_CMD_DISPATCH_H

#include "autoCode.h"

typedef struct
{
	const char *name;
	void (*func)(module_status_t *status, run_level_modules_count_t (*count)[RUN_LEVEL_COUNT]);

} initrc_cmd_t;

int initrcCmdDispatch(const char *cmd, module_status_t *status, run_level_modules_count_t (*level_count)[RUN_LEVEL_COUNT]);


#endif

