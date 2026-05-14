/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_rtc_ZS042_impl.h
 * @brief arduinoMega hal_rtc_ZS042_impl header declarations.
 *
 */

#ifndef HAL_RTC_ZS042_IMPL_H
#define HAL_RTC_ZS042_IMPL_H

#include <stdint.h>

typedef struct
{
	uint8_t seconds; // 0-59
	uint8_t minutes; // 0-59
	uint8_t hours; // 0-23
	uint8_t weekday; // 1-7
	uint8_t day; // 1-31
	uint8_t month; // 1-12
	uint8_t year; // 0-99
} hal_rtc_time_t;

void hal_rtcInit(void);
void hal_rtcStart(void);
void hal_rtcStop(void);
uint8_t hal_rtcRead(hal_rtc_time_t *t);
uint8_t hal_rtcWrite(const hal_rtc_time_t *t);

#endif
