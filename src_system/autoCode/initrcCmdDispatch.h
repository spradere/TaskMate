/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file initrcCmdDispatch.h
 * @brief autoCode initrcCmdDispatch header declarations.
 *
 */

#ifndef INITRCCMDDISPATCH_H
#define INITRCCMDDISPATCH_H

#include "autoCode.h"

typedef struct
{
	const char *name;
	void (*func)(module_item_t *mod);

} initrc_cmd_t;

int initrcCmdDispatch(const char *cmd, module_item_t *mod);


#endif

