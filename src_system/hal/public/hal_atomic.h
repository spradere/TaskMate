/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_atomic.h
 * @brief public hal_atomic header declarations.
 *
 */

#ifndef HAL_ATOMIC_H
#define HAL_ATOMIC_H

#if defined(ARCH_avr8)
	#include "hal/arch/avr8/hal_atomic_impl.h"
	#define HAL_ATOMIC
#endif

#if !defined(HAL_ATOMIC)
	#error "No hal implementation for atomic block on selected hardware target."
#endif
#endif
