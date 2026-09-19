/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_atomic.h
 * @brief atomic header declarations.
 *
 */

#ifndef AVR8_AVR8_ATOMIC_H
#define AVR8_AVR8_ATOMIC_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <avr/interrupt.h>
#include <avr/io.h>

#include <stdint.h>

/* ============================================================================
 * Private definitions
 * ========================================================================== */

typedef uint8_t avr8_atomic_state_t;

/* ============================================================================
 * Private inline implementation
 * ========================================================================== */

static inline __attribute__((always_inline)) avr8_atomic_state_t avr8_atomicStart(void)
{
	avr8_atomic_state_t state = SREG;
	cli();
	return state;
}

static inline __attribute__((always_inline)) void avr8_atomicEnd(avr8_atomic_state_t state)
{
	asm volatile("" ::: "memory");
	SREG = state;
}

#endif // AVR8_AVR8_ATOMIC_H
