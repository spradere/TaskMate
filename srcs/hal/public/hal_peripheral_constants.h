/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_peripheral_constants.h
 * @brief Peripheral constant selection header.
 *
 */

#ifndef HAL_PUBLIC_HAL_PERIPHERAL_CONSTANTS_H
#define HAL_PUBLIC_HAL_PERIPHERAL_CONSTANTS_H

/* ============================================================================
 * Target selection
 * ========================================================================== */

#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/at2560_peripheral_constants.h"
	#define HAL_PERIPHERAL_CONSTANTS
#endif

#if !defined(HAL_PERIPHERAL_CONSTANTS)
	#error "No hal implementation for peripheral constants on selected hardware target."
#endif

#endif // HAL_PUBLIC_HAL_PERIPHERAL_CONSTANTS_H
