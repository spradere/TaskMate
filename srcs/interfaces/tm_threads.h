/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_threads.h
 * @brief Thread status declarations.
 */

#ifndef INTERFACES_TM_THREADS_H
#define INTERFACES_TM_THREADS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Stores the encoded run level and thread lifecycle flags.
 */
typedef uint8_t tm_thread_status_t;

// Bits [2:0] contain the run level
/**
 * @brief Identifies status bits in the thread status word.
 */
typedef enum __attribute__((packed))
{
	THREAD_BIT_TYPE_USER = 3,
	THREAD_BIT_TYPE_SYS,
	THREAD_BIT_INITIALIZED,
	THREAD_BIT_DEAD,
	THREAD_BIT_YIELDED
} tm_thread_status_bit_t;
_Static_assert(sizeof(tm_thread_status_bit_t) == 1U, "tm_thread_status_bit_t must be one byte");

#endif // INTERFACES_TM_THREADS_H
