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
 * @file hal_stack.h
 * @brief hal stack handling
 *
 */

#ifndef HAL_STACK_IMPL_H
#define HAL_STACK_IMPL_H

#include <avr/io.h> // get SP
#include <stdint.h>

#include "hal/public/auto_hal_define.h"

//static inline __attribute__((always_inline)) void hal_setStackPointer(uintptr_t sp) { SP = sp; }
//static inline __attribute__((always_inline)) uintptr_t hal_getStackPointer(void) { return SP; }

static inline __attribute__((always_inline)) void hal_setStackPointer(hal_stack_word_t* sp) { SP = (uintptr_t)sp; }
static inline __attribute__((always_inline)) hal_stack_word_t *hal_getStackPointer(void) { return (hal_stack_word_t*)SP; }

#endif
