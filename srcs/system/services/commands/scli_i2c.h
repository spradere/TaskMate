/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_i2c.h
 * @brief I2C command declarations.
 */

#ifndef COMMANDS_SCLI_I2C_H
#define COMMANDS_SCLI_I2C_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Handle the SCLI I2C command.
 * @return true if the command completed successfully.
 */
bool i2cCommand(uint8_t argc, char *argv[]);

#endif // COMMANDS_SCLI_I2C_H
