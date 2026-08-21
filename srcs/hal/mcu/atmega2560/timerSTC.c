/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSTC.c
 * @brief timer stc implementation.
 *
 */

#include "timerSTC.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "interfaces/define.h"
#include "interfaces/drivers.h"
#include "interfaces/macros.h"

const uint16_t hal_timerSTC_OVERFLOW_COUNT =
	625; // Interrupt every 10ms (10.10^-3 x 16.10^6 )/256 = 625

static hal_timerSTCCallback_t stc_callback = NULL;
static hal_driver_status_t timer_stc_status;

void hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr) { stc_callback = func_ptr; }

static uint8_t hal_timerSTCInit(void)
{

	// Set up timer3 for RTC
	TM_WRITEBIT(TCCR3B, WGM32, CS32); // CTC mode, prescaler 256
	OCR3A = hal_timerSTC_OVERFLOW_COUNT;

	hal_timerSTCControl(TM_DRIVER_STATUS_SETBIT, TM_DRIVER_BIT_INIT);
	return 0;
}

static uint8_t hal_timerSTCStart(void)
{
	if( (hal_timerSTCControl(TM_DRIVER_STATUS_GETBIT, TM_DRIVER_BIT_INIT) == 0) ||
		(hal_timerSTCControl(TM_DRIVER_STATUS_GETBIT, TM_DRIVER_BIT_DEAD) != 0) )
	{
		return TM_DRIVER_UNKNOW;
	}

	// start by enabling interrupt
	TM_SETBIT(TIMSK3, OCIE3A);

	hal_timerSTCControl(TM_DRIVER_STATUS_SETBIT, TM_DRIVER_BIT_START);
	return 0;
}

static uint8_t hal_timerSTCStop(void)
{
	// stop by disabling interrupt
	TM_CLEARBIT(TIMSK3, OCIE3A);

	hal_timerSTCControl(TM_DRIVER_STATUS_CLEARBIT, TM_DRIVER_BIT_START);
	return 0;
}

ISR(TIMER3_COMPA_vect)
{
	// Software time counter callback
	if( stc_callback != NULL ) { stc_callback(); }
}

uint8_t hal_timerSTCControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case TM_DRIVER_CTRL_INIT:
			hal_timerSTCInit();
			break;
		case TM_DRIVER_CTRL_START:
			hal_timerSTCStart();
			break;
		case TM_DRIVER_CTRL_STOP:
			hal_timerSTCStop();
			break;
		case TM_DRIVER_STATUS_RLSET:
			timer_stc_status &= (hal_driver_status_t)~TM_DRIVER_RL_MASK;
			timer_stc_status |= val;
			return 0;
		case TM_DRIVER_STATUS_RLGET:
			return timer_stc_status &= TM_DRIVER_RL_MASK;
		case TM_DRIVER_STATUS_SETBIT:
			TM_SETBIT(timer_stc_status, val);
			return 0;
		case TM_DRIVER_STATUS_CLEARBIT:
			TM_CLEARBIT(timer_stc_status, val);
			return 0;
		case TM_DRIVER_STATUS_GETBIT:
			return TM_GETBIT(timer_stc_status, val);
		default:
			return TM_DRIVER_UNKNOW;
	}
	return 0;
}
