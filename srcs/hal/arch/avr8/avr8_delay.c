/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_delay.c
 * @brief AVR8 blocking delay implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "interfaces/hal_delay.h"

#include <util/delay.h>

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define AVR8DELAY_STEP_LONG_us 100U
#define AVR8DELAY_STEP_SHORT_us 10U

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void hal_delayMs(uint16_t milliseconds)
{
	while( milliseconds >= 1U )
	{
		_delay_ms(1U);
		--milliseconds;
	}
}

void hal_delayUs(uint16_t microseconds)
{
	/* Constant arguments keep avr-libc delays independent of runtime floating point. */
	while( microseconds >= AVR8DELAY_STEP_LONG_us )
	{
		_delay_us(AVR8DELAY_STEP_LONG_us);
		microseconds -= AVR8DELAY_STEP_LONG_us;
	}
	while( microseconds >= AVR8DELAY_STEP_SHORT_us )
	{
		_delay_us(AVR8DELAY_STEP_SHORT_us);
		microseconds -= AVR8DELAY_STEP_SHORT_us;
	}
	while( microseconds >= 1U )
	{
		_delay_us(1U);
		--microseconds;
	}
}
