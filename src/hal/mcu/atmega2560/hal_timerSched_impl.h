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

#ifndef HAL_TIMER_SCHED_IMPL_H
#define HAL_TIMER_SCHED_IMPL_H

#include "hal/public/auto_hal_define.h"

//typedef void (*hal_timerSchedCallback_t)(void);
//typedef hal_stack_word_t *(*hal_timerSchedCallback_t)(hal_stack_word_t *stack_pointer);
typedef hal_stack_word_t *hal_timerSchedCallback_func_t(hal_stack_word_t *stack_pointer);
typedef hal_timerSchedCallback_func_t *hal_timerSchedCallback_ptr_t;

void hal_timerSchedInit(void);
void hal_timerSchedStart(void);
void hal_timerSchedStop(void);
void hal_timerSchedSetCallback(hal_timerSchedCallback_ptr_t func_ptr);

#endif
