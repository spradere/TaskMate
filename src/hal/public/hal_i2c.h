/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_i2c.h
 * @brief public hal_i2c header declarations.
 *
 */

#ifndef HAL_I2C_H
#define HAL_I2C_H

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/mcu/atmega2560/hal_i2c_impl.h"
	#define HAL_I2C
#endif

#if !defined(HAL_I2C)
    #error "No hal implementation for i2c on selected hardware target."
#endif
#endif
