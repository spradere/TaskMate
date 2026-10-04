/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_stack.h
 * @brief Stack command declarations.
 */

#ifndef COMMANDS_SCLI_STACK_H
#define COMMANDS_SCLI_STACK_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Handle the SCLI stack command.
 * @return true if the command completed successfully.
 */
bool stackCommand(uint8_t argc, char *argv[]);

#endif // COMMANDS_SCLI_STACK_H
