/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file i2c.h
 * @brief i2c header declarations.
 *
 */

#ifndef HAL_PUBLIC_I2C_H
#define HAL_PUBLIC_I2C_H


#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/i2c.h"
	#define HAL_I2C
#endif

#if !defined(HAL_I2C)
	#error "No hal implementation for i2c on selected hardware target."
#endif

#endif // HAL_PUBLIC_I2C_H
