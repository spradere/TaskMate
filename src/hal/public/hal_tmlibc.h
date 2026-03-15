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

#ifndef HAL_TMLIBC_H
#define HAL_TMLIBC_H

#define HAL_TMLIBC 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/mcu/atmega2560/hal_tmlibc_impl.h"
	#undef HAL_TMLIBC
	#define HAL_TMLIBC 1
#endif

_Static_assert(HAL_TMLIBC, "No hal implementation for tm_libc selected.");
#endif
