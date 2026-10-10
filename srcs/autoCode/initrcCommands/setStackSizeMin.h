/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setStackSizeMin.h
 * @brief init.rc setStackSizeMin command declarations.
 */

#ifndef INITRCCOMMANDS_SET_STACK_SIZE_MIN_H
#define INITRCCOMMANDS_SET_STACK_SIZE_MIN_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "initrcCommand.h"

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Set the minimum accepted task stack size for this target.
 * @param command Parsed setStackSizeMin command.
 */
void initrcSetStackSizeMin(const initrc_command_t *command);

#endif // INITRCCOMMANDS_SET_STACK_SIZE_MIN_H
