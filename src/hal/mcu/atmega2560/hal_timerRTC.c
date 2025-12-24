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
 * @file hal_timerRTC.c
 * @brief hal real time cloc
 *
 */

#include "hal/mcu/atmega2560/hal_timerRTC.h"
#include <avr/io.h>
#include <util/atomic.h>
#include <avr/interrupt.h>
#include "sysCore/realTimeClock.h"

const int hal_timerRTC_OVERFLOW_COUNT = 625; // Interrupt every 10ms (10.10^-3 x 16.10^6 )/256 = 625

void hal_timerRTCInit(void)
{

	// Set up hal_timerRTC interrupt for RTC
	TCCR3B = (1 << WGM32) | (1 << CS32); // CTC mode, prescaler 256
	OCR3A = hal_timerRTC_OVERFLOW_COUNT;
}

void hal_timerRTCStart(void)
{
	// start by enabling INT
	TIMSK3 |= (uint8_t)(1u << OCIE3A);
}

void hal_timerRTCStop(void)
{
	// stop by disabling INT
	TIMSK3 &= (uint8_t)~(1u << OCIE3A);
}

ISR(TIMER3_COMPA_vect) { realTimeClock(); }
