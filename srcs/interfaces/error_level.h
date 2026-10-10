/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file error_level.h
 * @brief portable TaskMate error level definitions.
 */

#ifndef INTERFACES_ERROR_LEVEL_H
#define INTERFACES_ERROR_LEVEL_H

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Classifies how TaskMate handles an error.
 */
typedef enum __attribute__((packed))
{
	ERR_LEVEL_FLOW, // Normal flow interruption; handled by the thread.
	ERR_LEVEL_WARN, // Recoverable abnormal flow; handled by the thread and logged by the system.
	ERR_LEVEL_FAIL, // Component failure; handled by the system and logged persistently.
	ERR_LEVEL_PANIC // Critical problem; handled by the system through a controlled halt.
} err_level_t;
_Static_assert(sizeof(err_level_t) == 1U, "err_level_t must be one byte");

#endif // INTERFACES_ERROR_LEVEL_H
