/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_timerSched.h
 * @brief public hal_timerSched header declarations.
 *
 */

#ifndef HAL_TIMERSCHED_H
#define HAL_TIMERSCHED_H

#define HAL_TIMER_SCHED 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/mcu/atmega2560/hal_timerSched_impl.h"
	#undef HAL_TIMER_SCHED
	#define HAL_TIMER_SCHED 1
#endif

_Static_assert(HAL_TIMER_SCHED,
			   "No hal implementation for scheduler timer on selected hardware target.");
#endif
