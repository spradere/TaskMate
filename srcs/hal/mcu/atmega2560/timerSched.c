/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSched.c
 * @brief timer sched implementation.
 *
 */

#include "timerSched.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "hal/arch/avr8/context.h"
#include "hal/arch/avr8/interrupt.h"
#include "hal/arch/avr8/stack.h"
#include "interfaces/define.h"
#include "interfaces/drivers.h"
#include "interfaces/macros.h"

const uint16_t TIMER1_OVERFLOW_COUNT = 2000; // Interrupt every 1ms (1.10^-3 x 16.10^6 )/8 = 2000

static hal_timerSchedCallback_ptr_t sched_callback = NULL;
static hal_driver_status_t timer_sched_status;

static uint8_t hal_timerSchedGetStatus(void)
{
	if( TM_GETBIT(timer_sched_status, DRV_BIT_ERROR) != 0 ) { return DRV_STATE_ERROR; }
	if( TM_GETBIT(timer_sched_status, DRV_BIT_INIT) == 0 )
	{
		if( TM_GETBIT(timer_sched_status, DRV_BIT_START) == 0 ) { return DRV_STATE_OFF; }
		return DRV_STATE_ERROR;
	}
	if( TM_GETBIT(timer_sched_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

uint8_t hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr)
{
	if( hal_timerSchedGetStatus() >= DRV_STATE_INITIALIZED ) { return DRV_STATE_ERROR; }
	sched_callback = func_ptr;
	return 0;
}

uint8_t hal_timerSchedLoad(void)
{
	if( hal_timerSchedGetStatus() != DRV_STATE_RUNNING ) { return DRV_STATE_ERROR; }
#define LOAD_GUARD 4
	const uint16_t LOAD = TIMER1_OVERFLOW_COUNT - LOAD_GUARD;

	TCNT1L = LOAD & 0xFF;
	TCNT1H = LOAD >> 8;
	return 0;
}

static uint8_t hal_timerSchedInit(void)
{
	// Set up timer1 interrupt for scheduler
	TM_SETBIT(TCCR1B, WGM12); // CTC mode
	OCR1A = TIMER1_OVERFLOW_COUNT;
	TM_SETBIT(TIMSK1, OCIE1A); // output compare interrupt enable

	hal_timerSchedControl(DRV_CTRL_SETBIT, DRV_BIT_INIT);
	return 0;
}

#define TIMER_SCHED_START                              \
	"lds r24, %0\n\t"                                  \
	"ori r24, %1\n\t"                                  \
	"sts  %0, r24\n\t" : : "M"(_SFR_MEM_ADDR(TCCR1B)), \
						   "n"((uint8_t)(1u << CS11))  \
		: "r24"

static uint8_t hal_timerSchedStart(void)
{
	if( (hal_timerSchedControl(DRV_CTRL_GETBIT, DRV_BIT_INIT) == 0) ||
		(hal_timerSchedControl(DRV_CTRL_GETBIT, DRV_BIT_DEAD) != 0) )
	{
		return DRV_UNKNOW;
	}

	asm volatile(TIMER_SCHED_START);
	hal_timerSchedControl(DRV_CTRL_SETBIT, DRV_BIT_START);
	return 0;
}

#define TIMER_SCHED_STOP                              \
	"lds r24, %0\n\t"                                 \
	"andi r24, %1\n\t"                                \
	"sts  %0, r24\n\t"                                \
	"sts %2,r1 \n\t"                                  \
	"sts %3,r1 \n\t" : : "M"(_SFR_MEM_ADDR(TCCR1B)),  \
						 "n"((uint8_t)~(1u << CS11)), \
						 "M"(_SFR_MEM_ADDR(TCNT1H)),  \
						 "M"(_SFR_MEM_ADDR(TCNT1L))   \
		: "r24"

static uint8_t hal_timerSchedStop(void)
{
	asm volatile(TIMER_SCHED_STOP);
	hal_timerSchedControl(DRV_CTRL_CLEARBIT, DRV_BIT_START);
	return 0;
}

#define TM_SCHED_CALL_BACK                       \
	"in r24, 0x3d \n\t"                          \
	"in r25, 0x3e \n\t"                          \
	"lds r30, %0 \n\t"                           \
	"lds r31, %0+1 \n\t"                         \
	"movw r28,r24 \n\t"                          \
	"sbiw r30,0x00 \n\t"                         \
	"breq .+4 \n\t"                              \
	"eicall \n\t"                                \
	"out 0x3e, r25 \n\t"                         \
	"out 0x3d, r24 \n\t" : : "m"(sched_callback) \
		: "r24", "r25", "r30", "r31", "r28", "r29"

ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	asm volatile(AVR8_CONTEXT_SAVE);
	asm volatile(TIMER_SCHED_STOP);

	asm volatile(TM_SCHED_CALL_BACK);

	asm volatile(TIMER_SCHED_START);
	asm volatile(AVR8_CONTEXT_RESTORE);
	asm volatile("reti \n\t");
}

uint8_t hal_timerSchedControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case DRV_CTRL_INIT:
			return hal_timerSchedInit();
		case DRV_CTRL_START:
			return hal_timerSchedStart();
		case DRV_CTRL_STOP:
			return hal_timerSchedStop();
		case DRV_CTRL_RLSET:
			timer_sched_status &= (hal_driver_status_t)~DRV_RL_MASK;
			timer_sched_status |= val;
			return 0;
		case DRV_CTRL_RLGET:
			return timer_sched_status & DRV_RL_MASK;
		case DRV_CTRL_SETBIT:
			TM_SETBIT(timer_sched_status, val);
			return 0;
		case DRV_CTRL_CLEARBIT:
			TM_CLEARBIT(timer_sched_status, val);
			return 0;
		case DRV_CTRL_GETBIT:
			return TM_GETBIT(timer_sched_status, val);
		case DRV_CTRL_GETSTATUS:
			return hal_timerSchedGetStatus();
		default:
			return DRV_UNKNOW;
	}
}
