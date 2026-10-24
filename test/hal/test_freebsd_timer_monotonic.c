/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_freebsd_timer_monotonic.c
 * @brief Check FreeBSD monotonic timer ticks, pause, restart, and wrap.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

/* -----------------------------------------------
 * Host clock model
 * ---------------------------------------------*/

#define TEST_TIMER_TICK_NS 100000L
#define TEST_TIMER_SECOND_NS 1000000000L
#define TEST_TIMER_CLOCK_FAILURE (-1)
#define TEST_TIMER_TICKS_BEFORE_SECOND 10003U
#define TEST_TIMER_TICKS_AT_SECOND 10004U

static struct timespec test_now;
static bool test_clock_fails;
static uint8_t test_atomic_depth;

static int testClockGettime(clockid_t clock_id, struct timespec *time);

#define clock_gettime testClockGettime
#include "hal/host/freebsd/freebsd_timerMonotonic.c"
#undef clock_gettime

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * HAL and host clock dependencies
 * ---------------------------------------------*/

static int testClockGettime(clockid_t clock_id, struct timespec *time)
{
	assert(clock_id == CLOCK_MONOTONIC);
	if( test_clock_fails ) { return TEST_TIMER_CLOCK_FAILURE; }
	*time = test_now;
	return 0U;
}

hal_atomic_state_t hal_atomicStart(void)
{
	assert(test_atomic_depth == 0U);
	test_atomic_depth++;
	return test_atomic_depth;
}

void hal_atomicEnd(hal_atomic_state_t state)
{
	assert(state == test_atomic_depth);
	test_atomic_depth--;
}

_Noreturn void hal_halt(void) { abort(); }

/* -----------------------------------------------
 * Test cases
 * ---------------------------------------------*/

int main(void)
{
	hal_driver_control_data_t data;

	assert(hal_timerMonotonicRead() == 0U);
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_ERROR);
	assert(hal_timerMonotonicControl(DRV_CTRL_GETLASTERROR, &data) == DRV_STATE_OFF);
	assert(data.error == ERR_HAL_DRIVER_NOT_INITIALIZED);
	assert(hal_timerMonotonicControl(DRV_CTRL_INIT, NULL) == DRV_STATE_INITIALIZED);
	data.run_level = RL_RUN_CORE;
	assert(hal_timerMonotonicControl(DRV_CTRL_RLSET, &data) == DRV_STATE_INITIALIZED);
	assert(hal_timerMonotonicControl(DRV_CTRL_RLGET, &data) == DRV_STATE_INITIALIZED);
	assert(data.run_level == RL_RUN_CORE);

	test_clock_fails = true;
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_ERROR);
	assert(hal_timerMonotonicControl(DRV_CTRL_GETSTATUS, NULL) == DRV_STATE_INITIALIZED);
	test_clock_fails = false;
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	test_now.tv_nsec = TEST_TIMER_TICK_NS - 1U;
	assert(hal_timerMonotonicRead() == 0U);
	test_now.tv_nsec = TEST_TIMER_TICK_NS;
	assert(hal_timerMonotonicRead() == 1U);
	test_now.tv_nsec = TEST_TIMER_TICK_NS * 3U + TEST_TIMER_TICK_NS / 2U;
	assert(hal_timerMonotonicRead() == 3U);
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	assert(hal_timerMonotonicRead() == 3U);

	test_now.tv_nsec = TEST_TIMER_TICK_NS * 4U;
	assert(hal_timerMonotonicControl(DRV_CTRL_STOP, NULL) == DRV_STATE_INITIALIZED);
	test_now.tv_sec = 1U;
	test_now.tv_nsec = 0U;
	assert(hal_timerMonotonicRead() == 4U);
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	test_now.tv_nsec = TEST_TIMER_TICK_NS;
	assert(hal_timerMonotonicRead() == 5U);
	test_now.tv_nsec = TEST_TIMER_SECOND_NS - TEST_TIMER_TICK_NS;
	assert(hal_timerMonotonicRead() == TEST_TIMER_TICKS_BEFORE_SECOND);
	test_now.tv_sec = 2U;
	test_now.tv_nsec = 0U;
	assert(hal_timerMonotonicRead() == TEST_TIMER_TICKS_AT_SECOND);

	// The public counter wraps modulo 2^32 after the same number of ticks as AVR.
	assert(hal_timerMonotonicControl(DRV_CTRL_STOP, NULL) == DRV_STATE_INITIALIZED);
	timer_monotonic_ticks = UINT32_MAX - 1U;
	test_now.tv_sec = 3U;
	test_now.tv_nsec = 0U;
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	test_now.tv_nsec = TEST_TIMER_TICK_NS * 2U;
	assert(hal_timerMonotonicRead() == 0U);
	assert(test_atomic_depth == 0U);
	return 0;
}
