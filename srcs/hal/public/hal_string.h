/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_string.h
 * @brief String macro selection header.
 *
 */

#ifndef HAL_PUBLIC_HAL_STRING_H
#define HAL_PUBLIC_HAL_STRING_H

/* ============================================================================
 * Target selection
 * ========================================================================== */

#if defined(ARCH_avr8)
	#include "hal/arch/avr8/avr8_string_macro.h"
	#define HAL_STRING
#endif

#if !defined(HAL_STRING)
	#error "No hal implementation for string macros on selected hardware target."
#endif

#endif // HAL_PUBLIC_HAL_STRING_H
