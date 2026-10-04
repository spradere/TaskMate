/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_errors.h
 * @brief Error syscall declarations.
 *
 */

#ifndef SYSCALL_SC_ERRORS_H
#define SYSCALL_SC_ERRORS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/error_catalog.h"
#include "interfaces/error_level.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Associates an error message with its severity.
 */
typedef struct
{
	const tm_string_t *name;
	const err_level_t level;
} err_item_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Get the message for an error code.
 * @param num Error code index.
 * @return Stored message for the code.
 */
const tm_string_t *err_getMessage(uint8_t num);
/**
 * @brief Stop system execution.
 */
void sc_halt(void);

#endif // SYSCALL_SC_ERRORS_H
