/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_commands.h
 * @brief SCLI command table declarations.
 */

#ifndef SERVICES_SCLI_COMMANDS_H
#define SERVICES_SCLI_COMMANDS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

typedef bool (*scli_cmd_func_t)(uint8_t argc, char *argv[]);

typedef struct
{
	const char *name;
	scli_cmd_func_t func;
} scli_cmd_t;

extern const scli_cmd_t scli_commands[];

#endif // SERVICES_SCLI_COMMANDS_H
