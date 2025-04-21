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
 * @file cmdDispatch.h
 * @brief command dispatch header
 *
 * @todo nothing
 */

#ifndef LISTCMD_H
#define LISTCMD_H

#include "utility/autoCode_src/autoCode.h"

typedef struct
{
	const char *name;
	void (*func)(module_status_t *);

} cmd_t;

int cmdDispatch(const char *cmd, module_status_t *status);


#endif

