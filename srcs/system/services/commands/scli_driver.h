/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_driver.h
 * @brief Driver command declarations.
 */

#ifndef COMMANDS_SCLI_DRIVER_H
#define COMMANDS_SCLI_DRIVER_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Handle the SCLI driver command.
 * @return true if the command completed successfully.
 */
bool driverCommand(uint8_t argc, char *argv[]);

#endif // COMMANDS_SCLI_DRIVER_H
