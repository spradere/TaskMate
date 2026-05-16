/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file int.h
 * @brief avr8 hal_int_impl header declarations.
 *
 */

#ifndef HAL_INT_IMPL_H
#define HAL_INT_IMPL_H

#include <avr/io.h>

static inline __attribute__((always_inline)) void hal_returnFromInterupt(void)
{
	asm volatile("reti \n\t");
}

static inline __attribute__((always_inline)) void hal_setGlobalInterupt(void)
{
	asm volatile("sei \n\t");
}

static inline __attribute__((always_inline)) void hal_clearGlobalInterupt(void)
{
	asm volatile("cli \n\t");
}

#endif
