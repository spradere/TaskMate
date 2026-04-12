/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_stack.h
 * @brief public hal_stack header declarations.
 *
 */

#ifndef HAL_STACK_H
#define HAL_STACK_H

#if !defined(HAL_SYSTEM_CRITICAL_ALLOWED)
    #error "NOT ALLOWED INCLUDE : hal_stack.h"
#endif

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/arch/avr8/hal_stack_impl.h"
	#define HAL_STACK
#endif

#if !defined(HAL_STACK)
    #error "No hal implementation for stack pointer on selected hardware target."
#endif
#endif
