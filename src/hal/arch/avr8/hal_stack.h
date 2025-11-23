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
 * @file hal_stack.h
 * @brief hal stack handling
 *
 * @todo nothing
 */

#include <avr/io.h>

static inline __attribute__((always_inline)) void hal_setStackPointer(uintptr_t sp) { SP = sp; }

static inline __attribute__((always_inline)) uintptr_t hal_getStackPointer(void) { return SP; }
