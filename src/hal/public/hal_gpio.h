/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_gpio.h
 * @brief public hal_gpio header declarations.
 *
 */

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#if defined(MCU_atmega2560)
	#include "hal/mcu/atmega2560/hal_gpio_impl.h"
	#define HAL_GPIO
#endif

#if !defined(HAL_GPIO)
    #error "No hal implementation for gpio on selected hardware target."
#endif
#endif
