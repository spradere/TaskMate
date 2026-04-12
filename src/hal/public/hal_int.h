/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_int.h
 * @brief public hal_int header declarations.
 *
 */

#ifndef HAL_INT_H
#define HAL_INT_H

#if !defined(HAL_SYSTEM_CRITICAL_ALLOWED)
    #error "NOT ALLOWED INCLUDE : hal_int.h"
#endif

#define HAL_INT 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/arch/avr8/hal_int_impl.h"
	#undef HAL_INT
	#define HAL_INT 1
#endif

_Static_assert(HAL_INT, "No hal implementation for int on selected hardware target.");
#endif
