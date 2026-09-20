/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_modules.h
 * @brief modules header declarations.
 */

#ifndef SYSCORE_MODULES_H
#define SYSCORE_MODULES_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "interfaces/hal_context.h"
#include "interfaces/hal_drivers.h"
#include "interfaces/tm_modules.h"

/* ============================================================================
 * Public definitions
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

// Driver
typedef struct
{
	uint8_t address;

	hal_driver_state_t (*control)(hal_driver_control_t, hal_driver_control_data_t *);

} mod_driver_item_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

void mod_threadSetCurrent(uint8_t n);
uint8_t mod_threadGetCurrent(void);

void mod_threadSetSTC(uint16_t count);
uint16_t mod_threadGetSTC(void);

mod_driver_item_t *mod_driverGetPointer(uint8_t id);
mod_thread_item_t *mod_threadGetPointer(uint8_t id);

void mod_threadsAlloc(void);
void mod_driversAlloc(void);

#endif // SYSCORE_MODULES_H
