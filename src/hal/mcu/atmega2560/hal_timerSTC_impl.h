/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_timerSTC.h
 * @brief header hal real time clock
 *
 */

#ifndef HAL_TIMER_STC_H
#define HAL_TIMER_STC_H

typedef void (*hal_timerSTCCallback_t)(void);

void hal_timerSTCInit(void);
void hal_timerSTCStart(void);
void hal_timerSTCStop(void);
void hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr);

#endif
