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
 * @file hal_threadContextinit.h
 * @brief header hal thread context init
 *
 */

#include "hal/arch/avr8/arch_define.h" // get stack_word_t

void hal_threadContextInit(void (*func)(void), stack_word_t **stack_pointer, stack_word_t *stack_top);
