/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setErrorCountMax.h
 * @brief init.rc setErrorCountMax command declarations.
 */

#ifndef INITRCCOMMANDS_SET_ERROR_COUNT_MAX_H
#define INITRCCOMMANDS_SET_ERROR_COUNT_MAX_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "initrcCommand.h"

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Set the target error capacity and select the corresponding count type.
 * @param command Parsed setErrorCountMax command.
 */
void initrcSetErrorCountMax(const initrc_command_t *command);

#endif // INITRCCOMMANDS_SET_ERROR_COUNT_MAX_H
