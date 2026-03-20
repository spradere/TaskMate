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
 * @file hal_timerSched.h
 * @brief header hal part of scheduler
 *
 */

// @hal_system

#ifndef HAL_TIMER1_H
#define HAL_TIMER1_H

typedef void (*hal_timerSchedCallback_t)(void);

void hal_timerSchedInit(void);
void hal_timerSchedStart(void);
void hal_timerSchedStop(void);
void hal_timerSchedSetCallback(hal_timerSchedCallback_t func_ptr);

#endif
