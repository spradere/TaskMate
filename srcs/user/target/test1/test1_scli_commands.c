/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test1_scli_commands.c
 * @brief Test1 SCLI command table implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "test1_scli_commands.h"

#include "system/services/commands/date.h"
#include "system/services/commands/driver.h"
#include "system/services/commands/i2c.h"
#include "system/services/commands/stack.h"
#include "system/services/commands/thread.h"

/* =============================================================================
 * Public definitions
 * ===========================================================================*/

const scli_cmd_t scli_commands[] = {
	{"date", dateCommand},
	{"driver", driver},
	{"i2c", i2c},
	{"stack", stack},
	{"thread", thread},
	{0, 0},
};
