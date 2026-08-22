/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSTC.h
 * @brief timer stc header declarations.
 *
 */

#ifndef ATMEGA2560_TIMERSTC_H
#define ATMEGA2560_TIMERSTC_H

#include <stdint.h>

typedef void (*hal_timerSTCCallback_t)(void);

uint8_t hal_timerSTCControl(uint8_t cmd, uint8_t val);
uint8_t hal_timerSTCSetCallback(hal_timerSTCCallback_t func_ptr);

#endif // ATMEGA2560_TIMERSTC_H
