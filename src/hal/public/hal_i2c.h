/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_i2c.h
 * @brief public hal_i2c header declarations.
 *
 */

#ifndef HAL_I2C_H
#define HAL_I2C_H

#define HAL_I2C 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/mcu/atmega2560/hal_i2c_impl.h"
	#undef HAL_I2C
	#define HAL_I2C 1
#endif

_Static_assert(HAL_I2C, "No hal implementation for i2c on selected hardware target.");
#endif
