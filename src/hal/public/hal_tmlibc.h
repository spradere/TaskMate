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

#ifndef PHAL_TMLIBC_H
#define PHAL_TMLIBC_H

#define PHAL_TMLIBC 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/board/arduinoMega/hal_tmlibc.h"
	#undef PHAL_TMLIBC
	#define PHAL_TMLIBC 1
#endif

_Static_assert(PHAL_TMLIBC, "No hal implementation for tm_libc selected.");
#endif
