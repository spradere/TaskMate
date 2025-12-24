/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_timerScheduler.h
 * @brief header hal part of schuduler
 *
 */

#ifndef HAL_TIMERSCHEDULER_H
#define HAL_TIMERSCHEDULER_H

void hal_timerSchedulerInit(void);
void hal_timerSchedulerStart(void);
void hal_timerSchedulerStop(void);
void hal_timerSchedulerLoad(void);

#endif
