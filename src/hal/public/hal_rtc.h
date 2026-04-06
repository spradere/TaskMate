/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_rtc.h
 * @brief public hal_rtc_ZS042 header declarations.
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
