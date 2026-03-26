/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file modules.h
 * @brief module data base header
 */

#ifndef MODULES_H
#define MODULES_H

#include <stdint.h>

#include "hal/public/auto_hal_define.h" // get hal_stack_word_t from selected arch
#include "interfaces/tm_string_storage.h"
#include "sysCore/modules_define.h"

/*
 * Modules structures
 */

#define MOD_CANARY 0xa5a5

typedef struct
{
	const tm_string_t *name;
	uint8_t status;

	void (*main)(void);

	// TODO check if volatile is needed for software_time_counter
	uint16_t software_time_counter;

	hal_stack_word_t *stack_pointer;

	uint16_t canary_low;
	hal_stack_word_t stack[MOD_THREAD_STACK_SIZE];
	uint16_t canary_high;

} mod_thread_item_t;

// driver
typedef struct
{
	const tm_string_t *name;
	uint8_t status;

	void (*init)(void);
	void (*start)(void);
	void (*stop)(void);

} mod_driver_item_t;

/*
 * Modules functions
 */

void mod_threadSetCurrent(uint8_t n);
uint8_t mod_threadGetCurrent(void);

void mod_threadSetSTC(uint16_t count);
uint16_t mod_threadGetSTC(void);

mod_driver_item_t *mod_driverGetPointer(uint8_t id);
mod_thread_item_t *mod_threadGetPointer(uint8_t id);

void mod_threadsAlloc(void);
void mod_driversAlloc(void);

#endif
