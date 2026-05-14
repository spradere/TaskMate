/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_timerSched.h
 * @brief public hal_timerSched header declarations.
 *
 */

#ifndef HAL_TIMERSCHED_H
#define HAL_TIMERSCHED_H

#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/hal_timerSched_impl.h"
	#define HAL_TIMER_SCHED
#endif

#if !defined(HAL_TIMER_SCHED)
	#error "No hal implementation for scheduler timer on selected hardware target."
#endif
#endif
