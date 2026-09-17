/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_architecture_constants.h
 * @brief Architecture constant selection header.
 *
 */

#ifndef HAL_PUBLIC_HAL_ARCHITECTURE_CONSTANTS_H
#define HAL_PUBLIC_HAL_ARCHITECTURE_CONSTANTS_H

/* ============================================================================
 * Target selection
 * ========================================================================== */

#if defined(ARCH_avr8)
	#include "hal/arch/avr8/avr8_architecture_constants.h"
	#define HAL_ARCHITECTURE_CONSTANTS
#endif

#if !defined(HAL_ARCHITECTURE_CONSTANTS)
	#error "No hal implementation for architecture constants on selected hardware target."
#endif

#endif // HAL_PUBLIC_HAL_ARCHITECTURE_CONSTANTS_H
