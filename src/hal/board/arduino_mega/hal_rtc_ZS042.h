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
 * @brief header hal lcd implementation
 *
 */

// @hal_user

#ifndef HAL_ZS_042_H
#define HAL_ZS_042_H

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
