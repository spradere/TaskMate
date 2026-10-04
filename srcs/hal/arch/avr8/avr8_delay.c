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

#include "interfaces/hal_delay.h"

#include <util/delay.h>

void hal_delayMs(uint16_t milliseconds)
{
	while( milliseconds >= 1U )
	{
		_delay_ms(1);
		--milliseconds;
	}
}

void hal_delayUs(uint16_t microseconds)
{
	/* Constant arguments keep avr-libc delays independent of runtime floating point. */
	while( microseconds >= 100U )
	{
		_delay_us(100);
		microseconds -= 100U;
	}
	while( microseconds >= 10U )
	{
		_delay_us(10);
		microseconds -= 10U;
	}
	while( microseconds >= 1U )
	{
		_delay_us(1);
		--microseconds;
	}
}
