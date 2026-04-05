/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_context.h
 * @brief public hal_context header declarations.
 *
 */

#ifndef HAL_CONTEXT_H
#define HAL_CONTEXT_H

#define HAL_CONTEXT 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/arch/avr8/hal_context_impl.h"
	#undef HAL_CONTEXT
	#define HAL_CONTEXT 1
#endif

_Static_assert(HAL_CONTEXT,
			   "No hal implementation for context switch on selected hardware target.");
#endif
