/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file addScliCommand.h
 * @brief init.rc addScliCommand command declarations.
 */

#ifndef INITRCCOMMANDS_ADDSCLICOMMAND_H
#define INITRCCOMMANDS_ADDSCLICOMMAND_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "initrcCommand.h"

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Add an SCLI command from a parsed init.rc command.
 */
void initrcAddScliCommand(const initrc_command_t *command);

#endif // INITRCCOMMANDS_ADDSCLICOMMAND_H
