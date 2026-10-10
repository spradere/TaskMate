/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_threads_data.h
 * @brief Internal thread storage declarations.
 */

#ifndef SYSCORE_SYS_THREADS_DATA_H
#define SYSCORE_SYS_THREADS_DATA_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_context.h"
#include "interfaces/tm_mod.h"

/* ============================================================================
 * Internal definitions
 * ========================================================================== */

#define MOD_CANARY 0x5au
#define MOD_STACK_PATTERN 0xa5u
/* The configured stack size includes both boundary canary words. */
#define MOD_STACK_CANARY_WORD_COUNT 2u
#define MOD_STACK_FIRST_USABLE_INDEX 1u
#define MOD_STACK_LOW_CANARY_INDEX 0u

typedef struct
{
	volatile uint8_t status;
	uint8_t saved_run_level;

	void (*main)(void);

	volatile uint16_t software_time_counter;

	hal_context_t context;

	hal_stack_word_t *stack;
	uint16_t stack_size;

} mod_thread_item_t;

/* ============================================================================
 * Internal API
 * ========================================================================== */

mod_thread_item_t *mod_threadGetPointer(mod_count_t id);

#endif // SYSCORE_SYS_THREADS_DATA_H
