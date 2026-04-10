/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_atomic_impl.h
 * @brief avr8 hal_atomic_impl header declarations.
 *
 */

#ifndef HAL_ATOMIC_IMPL_H
#define HAL_ATOMIC_IMPL_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "hal/arch/avr8/hal_arch_define.h" // get hal_irq_state_t

static inline __attribute__((always_inline)) hal_irq_sate_t hal_atomicStart(void)
{
	hal_irq_sate_t state = SREG;
	cli();
	return state;
}

static inline __attribute__((always_inline)) void hal_atomicEnd(hal_irq_sate_t state)
{
	SREG = state;
}

#endif
