/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_ports_pins.h
 * @brief Port and pin type selection header.
 *
 */

#ifndef HAL_PUBLIC_HAL_PORTS_PINS_H
#define HAL_PUBLIC_HAL_PORTS_PINS_H

/* ============================================================================
 * Target selection
 * ========================================================================== */

#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/at2560_ports_pins.h"
	#define HAL_PORTS_PINS
#endif

#if !defined(HAL_PORTS_PINS)
	#error "No hal implementation for ports and pins on selected hardware target."
#endif

#endif // HAL_PUBLIC_HAL_PORTS_PINS_H
