/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_timerSTC.c
 * @brief hal real time clock
 *
 */

#include "hal/mcu/atmega2560/hal_timerSTC_impl.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>


#include "tm_libc/tm_syslog.h"

//#include "TaskMate.h"

//#include "sysCore/tm_softwareTimeCounter.h"

const int hal_timerSTC_OVERFLOW_COUNT = 625; // Interrupt every 10ms (10.10^-3 x 16.10^6 )/256 = 625

static hal_timerSTCCallback_t STC_callback = NULL;
void hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr)
{
	uintptr_t p = (uintptr_t)func_ptr;
	tm_syslog(TM_STR("[timer STC] callback = 0x%04x\n"), (p<<1));
	STC_callback = func_ptr;
}

void hal_timerSTCInit(void)
{

	// Set up hal_timerSTC interrupt for RTC
	TCCR3B = (1 << WGM32) | (1 << CS32); // CTC mode, pre scaler 256
	OCR3A = hal_timerSTC_OVERFLOW_COUNT;
}

void hal_timerSTCStart(void)
{
	// start by enabling interrupt
	TIMSK3 |= (uint8_t)(1u << OCIE3A);
}

void hal_timerSTCStop(void)
{
	// stop by disabling interrupt
	TIMSK3 &= (uint8_t)~(1u << OCIE3A);
}

ISR(TIMER3_COMPA_vect)
{
	// STC_callback
	if( STC_callback != NULL ) { STC_callback(); }
}
