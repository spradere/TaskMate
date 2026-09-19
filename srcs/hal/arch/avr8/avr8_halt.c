/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_halt.c
 * @brief AVR8 halt implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <avr/interrupt.h>

#include "interfaces/hal_halt.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

_Noreturn void hal_halt(void)
{
	cli();
	while( 1 ) {}
}
