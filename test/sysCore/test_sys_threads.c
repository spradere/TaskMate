/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_sys_threads.c
 * @brief Check thread identifiers before static database access.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include "hal/arch/avr8/avr8_types.h"
#include "system/sysCore/sys_threads.c"

#define HAL_STRING_RAM(string) \
	((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_RAM})
#define HAL_STRING_INROM(name, txt) \
	static const tm_string_t name = {.text = (const uint8_t *)(txt), .storage = TM_MEM_ROM}
#define HAL_STRING_COMPARE(left, right, size) \
	strncmp((const char *)(left).text, (const char *)(right).text, (size))

#include "system/sysCall/sc_threads.c"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TESTSYSTHREADS_DELAY_NEXTSTART_ms 500U

/* -----------------------------------------------
 * Test state
 * ---------------------------------------------*/

static uint8_t atomic_depth;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * HAL and generated thread dependencies
 * ---------------------------------------------*/

_Noreturn void hal_halt(void)
{
	assert(false);
	while( 1U );
}

void hal_threadContextInit(void (*func)(void), hal_context_t *context,
						   void *stack_top)
{
	(void)func;
	(void)context;
	(void)stack_top;
}

void system(void) {}
void scli(void) {}
void task1(void) {}
void task2(void) {}

hal_atomic_state_t hal_atomicStart(void)
{
	atomic_depth++;
	return atomic_depth;
}

void hal_atomicEnd(hal_atomic_state_t state)
{
	assert(state == atomic_depth);
	atomic_depth--;
}

tm_run_level_t tm_schedulerRunLevelGet(void) { return RL_RUN_CORE; }
bool tm_schedulerRunLevelSet(tm_run_level_t run_level)
{
	(void)run_level;
	return true;
}
void tm_schedulerCoop(void)
{
	TM_CLEARBIT(mod_threadGetPointer(mod_threadGetCurrent())->status, THREAD_BIT_YIELDED);
}

/* -----------------------------------------------
 * Test cases
 * ---------------------------------------------*/

int main(void)
{
	mod_threadsAlloc();
	assert(mod_threadGetPointer(0U) != NULL);
	assert(mod_threadGetPointer(MOD_THREAD_COUNT - 1U) != NULL);
	assert(mod_threadGetPointer(MOD_THREAD_COUNT) == NULL);
	assert(mod_threadGetPointer(UINT8_MAX) == NULL);

	assert(mod_threadSetCurrent(MOD_THREAD_COUNT - 1U));
	assert(mod_threadGetCurrent() == MOD_THREAD_COUNT - 1U);
	assert(!mod_threadSetCurrent(MOD_THREAD_COUNT));
	assert(mod_threadGetCurrent() == MOD_THREAD_COUNT - 1U);

	uint16_t count = 0U;
	assert(mod_threadSetSTC(1U));
	assert(mod_threadGetSTC(&count) && (count == 1U));
	assert(!mod_threadGetSTC(NULL));
	assert(mod_threadSetNextStartDelay_ms(TESTSYSTHREADS_DELAY_NEXTSTART_ms));
	assert(mod_threadGetPointer(MOD_THREAD_COUNT - 1U)->next_start_delay_ms ==
		   TESTSYSTHREADS_DELAY_NEXTSTART_ms);

	tm_run_level_t run_level;
	uint16_t size_bytes;
	uint16_t depth_bytes;
	bool yielded;
	assert(mod_threadRunLevelGet(0U, &run_level));
	assert(run_level == RL_RUN_CORE);
	assert(mod_threadGetPointer(0U)->status == (1U << THREAD_BIT_TYPE_SYS));
	assert(mod_threadStackSizeGet(0U, &size_bytes));
	assert(mod_threadStackDepthGet(0U, &depth_bytes));
	assert(!mod_threadRunLevelGet(MOD_THREAD_COUNT, &run_level));
	assert(!mod_threadStackSizeGet(MOD_THREAD_COUNT, &size_bytes));
	assert(!mod_threadStackDepthGet(MOD_THREAD_COUNT, &depth_bytes));
	assert(!mod_threadSetInitialized(MOD_THREAD_COUNT));
	assert(!mod_threadStart(MOD_THREAD_COUNT, RL_RUN_CORE));
	assert(!mod_threadStop(MOD_THREAD_COUNT));
	assert(!mod_threadSetYielded(MOD_THREAD_COUNT));
	assert(!mod_threadIsYielded(MOD_THREAD_COUNT, &yielded));
	assert(!mod_threadIsYielded(0U, NULL));
	mod_thread_item_t *task = mod_threadGetPointer(MOD_THREAD_COUNT - 1U);
	const tm_thread_status_t task_status = task->status;
	assert(mod_threadRunLevelGet(MOD_THREAD_COUNT - 1U, &run_level));
	assert(run_level == RL_RUN_USER);
	assert(mod_threadStop(MOD_THREAD_COUNT - 1U));
	assert(task->run_level == RL_RUN_NONE);
	assert(task->saved_run_level == RL_RUN_USER);
	assert(task->status == task_status);
	assert(mod_threadStart(MOD_THREAD_COUNT - 1U, RL_RUN_CORE));
	assert(task->run_level == RL_RUN_USER);
	assert(task->status == task_status);

	const tm_string_t *name;
	assert(sc_threadGetInfo(0U, &name, &run_level, &size_bytes));
	assert(sc_threadGetStackDepth(0U, &depth_bytes));
	assert(!sc_threadGetInfo(MOD_THREAD_COUNT, &name, &run_level, &size_bytes));
	assert(!sc_threadGetStackDepth(MOD_THREAD_COUNT, &depth_bytes));
	assert(!sc_threadGetSTCChecked(NULL));
	assert(!sc_threadStart("missing", RL_RUN_CORE));

	thread_current = MOD_THREAD_COUNT;
	assert(!sc_threadSetSTC(1U));
	assert(!sc_threadSetNextStartDelay_ms(1U));
	assert(!sc_threadGetSTCChecked(&count));
	assert(sc_threadGetSTC() == 0U);
	assert(!sc_threadSetInitialized());
	assert(!sc_coopYield());
	assert(atomic_depth == 0U);
	assert(mod_threadSetCurrent(0U));
	assert(sc_threadSetSTC(1U));
	assert(sc_threadSetNextStartDelay_ms(TESTSYSTHREADS_DELAY_NEXTSTART_ms));
	assert(mod_threadGetPointer(0U)->next_start_delay_ms == TESTSYSTHREADS_DELAY_NEXTSTART_ms);
	assert(sc_threadGetSTCChecked(&count) && (count == 1U));
	assert(sc_threadGetSTC() == 1U);
	assert(sc_threadSetInitialized());
	assert(sc_coopYield());
	assert(atomic_depth == 0U);

	return 0;
}
