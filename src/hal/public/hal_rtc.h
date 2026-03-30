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
 * @file hal_rtc_ZS042.h
 * @brief public header hal part of RTC
 *
 */

#ifndef HAL_RTC_H
#define HAL_RTC_H

#define HAL_RTC 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/board/arduinoMega/hal_rtc_ZS042_impl.h"
	#undef HAL_RTC
	#define HAL_RTC 1
#endif

_Static_assert(HAL_RTC, "No hal implementation for rtc on selected hardware target.");
#endif
