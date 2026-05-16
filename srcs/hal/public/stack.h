/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file stack.h
 * @brief public hal_stack header declarations.
 *
 */

#ifndef PUBLIC_STACK_H
#define PUBLIC_STACK_H

#if !defined(HAL_SYSTEM_CRITICAL_ALLOWED)
	#error "NOT ALLOWED INCLUDE : stack.h"
#endif

#if defined(ARCH_avr8)
	#include "hal/arch/avr8/stack.h"
	#define HAL_STACK
#endif

#if !defined(HAL_STACK)
	#error "No hal implementation for stack pointer on selected hardware target."
#endif
#endif
