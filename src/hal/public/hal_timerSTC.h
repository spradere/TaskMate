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

#ifndef HAL_TIMERSTC_H
#define HAL_TIMERSTC_H

#define HAL_TIMERSTC 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/mcu/atmega2560/hal_timerSTC_impl.h"
	#undef HAL_TIMERSTC
	#define HAL_TIMERSTC 1
#endif

_Static_assert(HAL_TIMERSTC,
			   "No hal implementation for Software Time Counter on selected hardware target.");
#endif
