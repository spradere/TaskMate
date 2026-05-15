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

#if defined(BOARD_arduinoMega)
	#include "board/arduinoMega/hal_rtc_ZS042_impl.h"
	#define HAL_RTC
#endif

#if !defined(HAL_RTC)
	#error "No hal implementation for rtc on selected hardware target."
#endif
#endif
