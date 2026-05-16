/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSTC.h
 * @brief atmega2560 hal_timerSTC_impl header declarations.
 *
 */

#ifndef HAL_TIMERSTC_IMPL_H
#define HAL_TIMERSTC_IMPL_H

typedef void (*hal_timerSTCCallback_t)(void);

void hal_timerSTCInit(void);
void hal_timerSTCStart(void);
void hal_timerSTCStop(void);
void hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr);

#endif
