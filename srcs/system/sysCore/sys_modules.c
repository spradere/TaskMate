/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_modules.c
 * @brief modules implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sys_modules.h"

#include "interfaces/tm_modules.h"
#include "system/sysCore/sys_modules_list.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static struct
{
	mod_driver_item_t drivers[MOD_DRIVER_COUNT];
	mod_thread_item_t threads[MOD_THREAD_COUNT];
	uint8_t thread_current;
} mod_data_base;

// [autoCode_tag] thread_stacks
#include "thread_stacks.inc"
// [/tag]

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Module database access
 * ---------------------------------------------*/

void mod_threadSetCurrent(uint8_t n) { mod_data_base.thread_current = n; }
uint8_t mod_threadGetCurrent(void) { return mod_data_base.thread_current; }

void mod_threadSetSTC(uint16_t count)
{
	mod_data_base.threads[mod_data_base.thread_current].software_time_counter = count;
}

uint16_t mod_threadGetSTC(void)
{
	return mod_data_base.threads[mod_data_base.thread_current].software_time_counter;
}

mod_driver_item_t *mod_driverGetPointer(uint8_t id) { return &mod_data_base.drivers[id]; }
mod_thread_item_t *mod_threadGetPointer(uint8_t id) { return &mod_data_base.threads[id]; }

static void mod_threadStackInit(mod_thread_item_t *thread)
{
	for( uint16_t stack_index = 0; stack_index < thread->stack_size; stack_index++ )
	{
		thread->stack[stack_index] = MOD_STACK_PATTERN;
	}
	thread->stack[MOD_STACK_LOW_CANARY_INDEX] = MOD_CANARY;
	thread->stack[thread->stack_size - MOD_STACK_FIRST_USABLE_INDEX] = MOD_CANARY;
}

/* -----------------------------------------------
 * Static module allocation
 * ---------------------------------------------*/

void mod_threadsAlloc(void)
{
	// [autoCode_tag] threads_alloc
#include "threads_alloc.inc"
	// [/tag]
}
void mod_driversAlloc(void)
{
	// [autoCode_tag] drivers_alloc
#include "drivers_alloc.inc"
	// [/tag]
}
