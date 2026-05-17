/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file lcd.h
 * @brief lcd header declarations.
 *
 */

#ifndef HAL_PUBLIC_LCD_H
#define HAL_PUBLIC_LCD_H

#if defined(HWT_test1)
	#include "hal/drivers/lcd_AMC2004/lcd_AMC2004.h"
	#define HAL_LCD
#endif

#if !defined(HAL_LCD)
	#error "No hal implementation for lcd on selected hardware target."
#endif

#endif // HAL_PUBLIC_LCD_H
