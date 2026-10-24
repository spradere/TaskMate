/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file setInitrcVersion.h
 * @brief init.rc setInitrcVersion command declarations.
 */

#ifndef INITRCCOMMANDS_SET_INITRC_VERSION_H
#define INITRCCOMMANDS_SET_INITRC_VERSION_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "initrcCommand.h"

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Validate the init.rc version command.
 * @return true if the syntax version matches autoCode.
 */
bool initrcSetInitrcVersion(const initrc_command_t *command);

#endif // INITRCCOMMANDS_SET_INITRC_VERSION_H
