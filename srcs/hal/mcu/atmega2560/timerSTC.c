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

static uint8_t hal_timerSTCGetStatus(void)
{
	if( TM_GETBIT(timer_stc_status, DRV_BIT_ERROR) != 0 ) { return DRV_STATE_ERROR; }
	if( TM_GETBIT(timer_stc_status, DRV_BIT_INIT) == 0 )
	{
		if( TM_GETBIT(timer_stc_status, DRV_BIT_START) == 0 ) { return DRV_STATE_OFF; }
		return DRV_STATE_ERROR;
	}
	if( TM_GETBIT(timer_stc_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

uint8_t hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr)
{
	if( hal_timerSTCGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
	stc_callback = func_ptr;
	return 0;
}

static uint8_t hal_timerSTCInit(void)
{

	// Set up timer3 for RTC
	TM_WRITEBIT(TCCR3B, WGM32, CS32); // CTC mode, prescaler 256
	OCR3A = hal_timerSTC_OVERFLOW_COUNT;

	hal_timerSTCControl(DRV_CTRL_SETBIT, DRV_BIT_INIT);
	return 0;
}

static uint8_t hal_timerSTCStart(void)
{
	if( (hal_timerSTCControl(DRV_CTRL_GETBIT, DRV_BIT_INIT) == 0) ||
		(hal_timerSTCControl(DRV_CTRL_GETBIT, DRV_BIT_DEAD) != 0) )
	{
		return DRV_UNKNOW;
	}

	// start by enabling interrupt
	TM_SETBIT(TIMSK3, OCIE3A);

	hal_timerSTCControl(DRV_CTRL_SETBIT, DRV_BIT_START);
	return 0;
}

static uint8_t hal_timerSTCStop(void)
{
	// stop by disabling interrupt
	TM_CLEARBIT(TIMSK3, OCIE3A);

	hal_timerSTCControl(DRV_CTRL_CLEARBIT, DRV_BIT_START);
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
		case DRV_CTRL_INIT:
			return hal_timerSTCInit();
		case DRV_CTRL_START:
			return hal_timerSTCStart();
		case DRV_CTRL_STOP:
			return hal_timerSTCStop();
		case DRV_CTRL_RLSET:
			timer_stc_status &= (hal_driver_status_t)~DRV_RL_MASK;
			timer_stc_status |= val;
			return 0;
		case DRV_CTRL_RLGET:
			return timer_stc_status & DRV_RL_MASK;
		case DRV_CTRL_SETBIT:
			TM_SETBIT(timer_stc_status, val);
			return 0;
		case DRV_CTRL_CLEARBIT:
			TM_CLEARBIT(timer_stc_status, val);
			return 0;
		case DRV_CTRL_GETBIT:
			return TM_GETBIT(timer_stc_status, val);
		case DRV_CTRL_GETSTATUS:
			return hal_timerSTCGetStatus();
		default:
			return DRV_UNKNOW;
	}
}
