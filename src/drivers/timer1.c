/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file timer1.c
 * @brief implementation of timer1 used for preemptive sheduler
 *
 * @todo Nothing
 */

#include <avr/io.h>
#include "sysCore/TaskMate_public.h"
#include "drivers/timer1.h"

uint8_t timer1_name[] = "timer 1";
uint8_t timer1_status = 0;

void timer1SetStatus(uint8_t status) { timer1_status = status; }

uint8_t timer1GetStatus(void) { return timer1_status; }

uint8_t *timer1GetName(void) { return &timer1_name[0]; }

void timer1Init(void)
{
	// Set up timer1 interrupt for scheduler
	TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
	OCR1A = 1999; // Interrupt every 1ms

	// Set output for in board led 13
	LED_DDR |= (1 << LED_PIN);
}

void timer1Start(void)
{
	// start by enable INT
	TIMSK1 |= (1 << OCIE1A);
}

void timer1Stop(void)
{
	// nothing to do, will stop all system.
}
