/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_at2560_timer_monotonic.c
 * @brief Check Timer3 setup, driver life cycle, and atomic tick reads.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdint.h>

/* -----------------------------------------------
 * Host register model
 * ---------------------------------------------*/

#define WGM32 3U
#define CS31 1U
#define OCIE3A 1U
#define OCF3A 1U

static volatile uint8_t TCCR3A, TCCR3B, TIMSK3, TIFR3;
static volatile uint16_t TCNT3, OCR3A;

#include "interfaces/hal_atomic.h"

static uint8_t atomic_start_count;
static uint8_t atomic_end_count;
static hal_atomic_state_t restored_state;

hal_atomic_state_t hal_atomicStart(void)
{
	atomic_start_count++;
	return 1U;
}

void hal_atomicEnd(hal_atomic_state_t state)
{
	atomic_end_count++;
	restored_state = state;
}

#include "hal/mcu/atmega2560/at2560_timerMonotonic.c"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	hal_driver_control_data_t data;

	assert(hal_timerMonotonicControl(DRV_CTRL_GETSTATUS, NULL) == DRV_STATE_OFF);
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_ERROR);
	assert(hal_timerMonotonicControl(DRV_CTRL_GETLASTERROR, &data) == DRV_STATE_OFF);
	assert(data.error == ERR_HAL_DRIVER_NOT_INITIALIZED);

	assert(hal_timerMonotonicControl(DRV_CTRL_INIT, NULL) == DRV_STATE_INITIALIZED);
	assert(TCCR3A == 0U);
	assert(TCCR3B == (1U << WGM32));
	assert(TCNT3 == 0U);
	assert(OCR3A == 199U);
	assert((TIMSK3 & (1U << OCIE3A)) == 0U);

	data.run_level = RL_RUN_CORE;
	assert(hal_timerMonotonicControl(DRV_CTRL_RLSET, &data) == DRV_STATE_INITIALIZED);
	assert(hal_timerMonotonicControl(DRV_CTRL_RLGET, &data) == DRV_STATE_INITIALIZED);
	assert(data.run_level == RL_RUN_CORE);

	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	assert((TCCR3B & (1U << CS31)) != 0U);
	assert((TIMSK3 & (1U << OCIE3A)) != 0U);
	TIMER3_COMPA_vect();
	TIMER3_COMPA_vect();
	assert(hal_timerMonotonicRead() == 2U);
	assert(atomic_start_count == 1U);
	assert(atomic_end_count == 1U);
	assert(restored_state == 1U);

	assert(hal_timerMonotonicControl(DRV_CTRL_STOP, NULL) == DRV_STATE_INITIALIZED);
	assert((TCCR3B & (1U << CS31)) == 0U);
	assert((TIMSK3 & (1U << OCIE3A)) == 0U);
	assert(hal_timerMonotonicRead() == 2U);
	assert(hal_timerMonotonicControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	assert(hal_timerMonotonicRead() == 2U);
	return 0;
}
