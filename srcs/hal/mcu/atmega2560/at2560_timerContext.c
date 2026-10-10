/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_timerContext.c
 * @brief ATmega2560 context-switch timer implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

#include "hal/arch/avr8/avr8_context.h"
#include "interfaces/drv_timerContext.h"
#include "interfaces/tm_define.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define AT2560TIMERCONTEXT_COUNT_OVERFLOW 1999U // Interrupt every 1 ms
#define AT2560TIMERCONTEXT_COUNT_GUARD 4U
#define AT2560TIMERCONTEXT_BIT_CS10 0U
#define AT2560TIMERCONTEXT_BIT_CS11 1U
#define AT2560TIMERCONTEXT_BIT_CS12 2U
#define AT2560TIMERCONTEXT_ADDR_TCCR1B 0x81U
#define AT2560TIMERCONTEXT_ADDR_TCNT1H 0x85U
#define AT2560TIMERCONTEXT_ADDR_TCNT1L 0x84U

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static hal_timerContextCallback_ptr_t sched_callback = NULL;
static hal_context_t scheduler_context;
static hal_driver_status_t timer_context_status;
static err_codes_t timer_context_last_error = ERR_NO_ERROR;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Driver state
 * ---------------------------------------------*/

static hal_driver_state_t timerContextSetError(err_codes_t error)
{
	timer_context_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t hal_timerContextGetStatus(void)
{
	if( TM_GETBIT(timer_context_status, DRV_BIT_DEAD) != 0U )
	{
		timer_context_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_ERROR) != 0U )
	{
		return timerContextSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_INIT) == 0U )
	{
		if( TM_GETBIT(timer_context_status, DRV_BIT_START) == 0U ) { return DRV_STATE_OFF; }
		return timerContextSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_START) == 0U ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t timerContextRequireRunning(void)
{
	hal_driver_state_t state = hal_timerContextGetStatus();
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		return timerContextSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return state;
}

/* -----------------------------------------------
 * Callback and scheduler trigger
 * ---------------------------------------------*/

hal_driver_state_t hal_timerContextSetCallback(hal_timerContextCallback_ptr_t func_ptr)
{
	if( func_ptr == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
	sched_callback = func_ptr;
	return hal_timerContextGetStatus();
}

hal_driver_state_t hal_timerContextLoad(void)
{
	hal_driver_state_t state = timerContextRequireRunning();
	if( state != DRV_STATE_RUNNING ) { return state; }
	const uint16_t LOAD = AT2560TIMERCONTEXT_COUNT_OVERFLOW - AT2560TIMERCONTEXT_COUNT_GUARD;

	TCNT1 = LOAD;
	return DRV_STATE_RUNNING;
}

/* -----------------------------------------------
 * Driver life cycle
 * ---------------------------------------------*/

static hal_driver_state_t hal_timerContextInit(void)
{
	if( TM_GETBIT(timer_context_status, DRV_BIT_DEAD) != 0U )
	{
		return timerContextSetError(ERR_HAL_DRIVER_DEAD);
	}
	// Configure CTC mode while the clock is stopped, then load TOP from a known count.
	TCCR1A = 0U;
	TM_WRITEBIT(TCCR1B, WGM12);
	TCNT1 = 0U;
	OCR1A = AT2560TIMERCONTEXT_COUNT_OVERFLOW;
	TM_CLEARBIT(TIMSK1, OCIE1A);
	TM_WRITEBIT(TIFR1, OCF1A);

	TM_SETBIT(timer_context_status, DRV_BIT_INIT);
	timer_context_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

// contractual fixing of magic number
_Static_assert(CS10 == AT2560TIMERCONTEXT_BIT_CS10, "Unexpected CS10 position");
_Static_assert(CS11 == AT2560TIMERCONTEXT_BIT_CS11, "Unexpected CS11 position");
_Static_assert(CS12 == AT2560TIMERCONTEXT_BIT_CS12, "Unexpected CS12 position");
_Static_assert(_SFR_MEM_ADDR(TCCR1B) == AT2560TIMERCONTEXT_ADDR_TCCR1B,
			   "Unexpected TCCR1B address");
_Static_assert(_SFR_MEM_ADDR(TCNT1H) == AT2560TIMERCONTEXT_ADDR_TCNT1H,
			   "Unexpected TCNT1H address");
_Static_assert(_SFR_MEM_ADDR(TCNT1L) == AT2560TIMERCONTEXT_ADDR_TCNT1L,
			   "Unexpected TCNT1L address");

// Start timer1 by enabling prescaler=8
#define TIMER_CONTEXT_START      \
	"lds r24, 0x81\n\t"      \
	"ori r24, 0x02\n\t" 	\
	"sts  0x81, r24\n\t"	\

static hal_driver_state_t hal_timerContextStart(void)
{
	if( TM_GETBIT(timer_context_status, DRV_BIT_DEAD) != 0U )
	{
		return timerContextSetError(ERR_HAL_DRIVER_DEAD);
	}
	if( TM_GETBIT(timer_context_status, DRV_BIT_INIT) == 0U )
	{

		return timerContextSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}

	// Clear a stale compare before enabling the interrupt and the divide-by-8 clock.
	TCNT1 = 0U;
	TM_WRITEBIT(TIFR1, OCF1A);
	TM_SETBIT(TIMSK1, OCIE1A);
	asm volatile(TIMER_CONTEXT_START);
	TM_SETBIT(timer_context_status, DRV_BIT_START);
	return DRV_STATE_RUNNING;
}

// stop timer by setting CS1x = 0

#define TIMER_CONTEXT_STOP                             \
	"clr r1 \n\t"                                    \
	"lds r24, 0x81\n\t"                            \
	"andi r24, 0xF8\n\t" 							\
	"sts  0x81, r24\n\t"                           \
	"sts 0x85,r1 \n\t"                             \
	"sts 0x84,r1 \n\t"							 \

static hal_driver_state_t hal_timerContextStop(void)
{
	asm volatile(TIMER_CONTEXT_STOP);
	TM_CLEARBIT(TIMSK1, OCIE1A);
	TM_WRITEBIT(TIFR1, OCF1A);
	TM_CLEARBIT(timer_context_status, DRV_BIT_START);
	return hal_timerContextGetStatus();
}

/* -----------------------------------------------
 * Context-switch interrupt
 * ---------------------------------------------*/

_Static_assert(offsetof(hal_context_t, stack_pointer) == 0U,
			   "stack_pointer must be first in scheduler_context_t");

static hal_context_t *__attribute__((noinline, used)) schedWrapper(void)
{
	if( sched_callback != NULL ) { return sched_callback(&scheduler_context); }

	return &scheduler_context;
}

#define TM_SCHED_CALLBACK               \
	"in r18, 0x3d \n\t"                 \
	"in r19, 0x3e \n\t"                 \
	"sts scheduler_context, r18 \n\t"   \
	"sts scheduler_context+1, r19 \n\t" \
	"call schedWrapper\n\t"             \
	"movw r30, r24 \n\t"                \
	"ld r18, Z+ \n\t"                   \
	"ld r19, Z \n\t"                    \
	"out 0x3e, r19 \n\t"                \
	"out 0x3d, r18 \n\t"				\

/*
 * Naked ISR: save the interrupted context before any C code runs, switch stacks through
 * the scheduler callback, then restore registers and return to the selected thread.
 */
ISR(TIMER1_COMPA_vect, ISR_NAKED)
{
	asm volatile(
		AVR8_CONTEXT_SAVE TIMER_CONTEXT_STOP TM_SCHED_CALLBACK TIMER_CONTEXT_START AVR8_CONTEXT_RESTORE
		"reti \n\t");
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

hal_driver_state_t hal_timerContextControl(hal_driver_control_t command,
										 hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return hal_timerContextInit();
		case DRV_CTRL_START:
			return hal_timerContextStart();
		case DRV_CTRL_STOP:
			return hal_timerContextStop();
		// Keep lifecycle flags when updating the run-level bits.
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			timer_context_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			timer_context_status |= data->run_level;
			return hal_timerContextGetStatus();
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			data->run_level = RL_GET_RUN_LEVEL(timer_context_status);
			return hal_timerContextGetStatus();
		// Limit bit operations to the shared driver status flags.
		case DRV_CTRL_SETBIT:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_SETBIT(timer_context_status, data->status_bit);
			return hal_timerContextGetStatus();
		case DRV_CTRL_CLEARBIT:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_CLEARBIT(timer_context_status, data->status_bit);
			return hal_timerContextGetStatus();
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return timerContextSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			data->bit_value = TM_GETBIT(timer_context_status, data->status_bit) != 0U;
			return hal_timerContextGetStatus();
		// Expose current state and the most recent driver error separately.
		case DRV_CTRL_GETSTATUS:
			return hal_timerContextGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return timerContextSetError(ERR_NULL_POINTER); }
			data->error = timer_context_last_error;
			return hal_timerContextGetStatus();
		default:
			return timerContextSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
