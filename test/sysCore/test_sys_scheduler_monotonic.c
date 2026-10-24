/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_sys_scheduler_monotonic.c
 * @brief Check the selected thread's last start tick is recorded.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "hal/arch/avr8/avr8_types.h"
#include "interfaces/tm_driver_have.h"
#undef TM_DRIVER_HAVE_TIMER_MONOTONIC
#include "system/sysCore/sys_scheduler.c"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TEST_SCHEDULER_STACK_WORDS 4U
#define TEST_SCHEDULER_FIRST_TICK 123U
#define TEST_SCHEDULER_SECOND_TICK 124U
#define TEST_SCHEDULER_DELAY_ms 25U

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static mod_thread_item_t test_threads[MOD_THREAD_COUNT];
static hal_stack_word_t test_stacks[MOD_THREAD_COUNT][TEST_SCHEDULER_STACK_WORDS];
static mod_count_t test_current;
static uint32_t test_tick;
static uint8_t test_read_count;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Scheduler dependencies
 * ---------------------------------------------*/

_Noreturn void hal_halt(void) { abort(); }

_Noreturn void hal_contextStart(const hal_context_t *context)
{
	(void)context;
	abort();
}

hal_driver_state_t hal_timerContextControl(hal_driver_control_t command,
										   hal_driver_control_data_t *data)
{
	(void)command;
	(void)data;
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_timerContextSetCallback(hal_timerContextCallback_ptr_t callback)
{
	(void)callback;
	return DRV_STATE_INITIALIZED;
}

hal_driver_state_t hal_timerContextLoad(void) { return DRV_STATE_RUNNING; }

uint32_t hal_timerMonotonicRead(void)
{
	test_read_count++;
	return test_tick;
}

mod_count_t mod_threadGetCurrent(void) { return test_current; }

bool mod_threadSetCurrent(mod_count_t id)
{
	if( id >= MOD_THREAD_COUNT ) { return false; }
	test_current = id;
	return true;
}

mod_thread_item_t *mod_threadGetPointer(mod_count_t id)
{
	if( id >= MOD_THREAD_COUNT ) { return NULL; }
	return &test_threads[id];
}

void mod_threadTickSTC(void) {}

/* -----------------------------------------------
 * Test cases
 * ---------------------------------------------*/

int main(void)
{
	for( mod_count_t id = 0U; id < MOD_THREAD_COUNT; id++ )
	{
		test_threads[id].stack = test_stacks[id];
		test_threads[id].stack_size = TEST_SCHEDULER_STACK_WORDS;
		test_stacks[id][MOD_STACK_LOW_CANARY_INDEX] = MOD_CANARY;
		test_stacks[id][TEST_SCHEDULER_STACK_WORDS - MOD_STACK_FIRST_USABLE_INDEX] = MOD_CANARY;
	}
	test_threads[0U].run_level = RL_RUN_CORE;
	test_threads[1U].run_level = RL_RUN_CORE;
	test_threads[1U].next_start_delay_ms = TEST_SCHEDULER_DELAY_ms;

	test_tick = TEST_SCHEDULER_FIRST_TICK;
	assert(tm_schedulerRR(&test_threads[0U].context) == &test_threads[1U].context);
	assert(test_current == 1U);
	assert(test_threads[1U].last_start_ticks_100us == TEST_SCHEDULER_FIRST_TICK);
	assert(test_threads[0U].last_start_ticks_100us == 0U);
	assert(test_threads[1U].next_start_delay_ms == TEST_SCHEDULER_DELAY_ms);
	assert(test_read_count == 1U);

	test_tick = TEST_SCHEDULER_SECOND_TICK;
	assert(tm_schedulerRR(&test_threads[1U].context) == &test_threads[0U].context);
	assert(test_current == 0U);
	assert(test_threads[0U].last_start_ticks_100us == TEST_SCHEDULER_SECOND_TICK);
	assert(test_threads[1U].last_start_ticks_100us == TEST_SCHEDULER_FIRST_TICK);
	assert(test_read_count == 2U);
	return 0;
}
