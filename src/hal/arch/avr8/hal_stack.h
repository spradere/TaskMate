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
 * @brief hal stack
 *
 * @todo nothing
 */

#include <avr/io.h>

inline __attribute__((always_inline)) void hal_setStackPointer(uintptr_t sp)
{
    /*__asm__ __volatile__(
        "out __SP_L__, %A0    \n\t"
        "out __SP_H__, %B0    \n\t"
        :
        : "r"(sp)
    );*/
    SP = sp;
}

inline __attribute__((always_inline)) uintptr_t hal_getStackPointer(void)
{
    /*uintptr_t sp;
    __asm__ __volatile__(
        "in %A0, __SP_L__     \n\t"
        "in %B0, __SP_H__     \n\t"
        : "=r"(sp)
    );*/
    return SP;
}
