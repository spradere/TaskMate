/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file timerSched.h
 * @brief timer sched header declarations.
 *
 */

#ifndef ATMEGA2560_TIMERSCHED_H
#define ATMEGA2560_TIMERSCHED_H

#include <stdint.h>

#include "hal/arch/avr8/arch_define.h"

typedef hal_stack_word_t *hal_timerSchedCallback_func_t(hal_stack_word_t *stack_pointer);
typedef hal_timerSchedCallback_func_t *hal_timerSchedCallback_ptr_t;

uint8_t hal_timerSchedControl(uint8_t cmd, uint8_t val);
uint8_t hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr);
uint8_t hal_timerSchedLoad(void);

#endif // ATMEGA2560_TIMERSCHED_H
