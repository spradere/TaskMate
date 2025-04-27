/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file timer3.c
 * @brief implementation of timer3 used for Real Time Clock
 *
 * @todo Nothing
 */

#include <avr/io.h>
#include "sysCore/TaskMate_public.h"
#include "drivers/timer3.h"

// status
uint8_t timer3_status = 0;

void timer3SetStatus(uint8_t status) { timer3_status = status; }
uint8_t timer3GetStatus(void) { return timer3_status; }

void timer3Init(void)
{

	// Set up timer3 interrupt for RTC
	TCCR3B |= (1 << WGM32) | (1 << CS32); // CTC mode, prescaler 256
	OCR3A = 624; // Interrupt every 10ms
}

void timer3Start(void)
{
	// start by enable INT
	TIMSK3 |= (1 << OCIE3A);
}

void timer3Stop(void)
{
	// stop by disable INT
	TIMSK3 &= ~(1 << OCIE3A);
}
