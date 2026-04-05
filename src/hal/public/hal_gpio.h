/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_gpio.h
 * @brief public hal_gpio header declarations.
 *
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#define HAL_GPIO 0

#if defined(ARCH_avr8) && defined(MCU_atmega2560) && defined(BOARD_arduinoMega)
	#include "hal/mcu/atmega2560/hal_gpio_impl.h"
	#undef HAL_GPIO
	#define HAL_GPIO 1
#endif

_Static_assert(HAL_GPIO, "No hal implementation for gpio on selected hardware target.");
#endif
