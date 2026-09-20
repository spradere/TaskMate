/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file ucontext_scli_commands.c
 * @brief ucontext SCLI command table implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "ucontext_scli_commands.h"

#include "system/services/commands/driver.h"
#include "system/services/commands/stack.h"
#include "system/services/commands/thread.h"

/* =============================================================================
 * Public definitions
 * ===========================================================================*/

const scli_cmd_t scli_commands[] = {
	{"driver", driver},
	{"stack", stack},
	{"thread", thread},
	{0, 0},
};
