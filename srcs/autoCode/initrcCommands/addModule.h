/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file addModule.h
 * @brief init.rc addModule command declarations.
 */

#ifndef INITRCCOMMANDS_ADDMODULE_H
#define INITRCCOMMANDS_ADDMODULE_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "initrcCommand.h"

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Add a module from a parsed init.rc command.
 */
void initrcAddModule(const initrc_command_t *command);

#endif // INITRCCOMMANDS_ADDMODULE_H
