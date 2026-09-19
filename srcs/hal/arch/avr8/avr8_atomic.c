/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_atomic.c
 * @brief AVR8 atomic section contract implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "avr8_atomic.h"

#include "interfaces/hal_atomic.h"

_Static_assert(sizeof(hal_atomic_state_t) >= sizeof(avr8_atomic_state_t),
			   "hal_atomic_state_t cannot hold the AVR8 interrupt state");

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

hal_atomic_state_t hal_atomicStart(void) { return (hal_atomic_state_t)avr8_atomicStart(); }

void hal_atomicEnd(hal_atomic_state_t state)
{
	avr8_atomicEnd((avr8_atomic_state_t)state);
}
