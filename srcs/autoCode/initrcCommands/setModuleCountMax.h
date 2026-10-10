/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setModuleCountMax.h
 * @brief init.rc setModuleCountMax command declarations.
 */

#ifndef INITRCCOMMANDS_SET_MODULE_COUNT_MAX_H
#define INITRCCOMMANDS_SET_MODULE_COUNT_MAX_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "initrcCommand.h"

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Set the target module capacity and select the corresponding count type.
 * @param command Parsed setModuleCountMax command.
 */
void initrcSetModuleCountMax(const initrc_command_t *command);

#endif // INITRCCOMMANDS_SET_MODULE_COUNT_MAX_H
