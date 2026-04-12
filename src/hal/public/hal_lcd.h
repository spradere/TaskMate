/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_lcd.h
 * @brief public hal_lcd_AMC2004 header declarations.
 *
 */

#ifndef HAL_LCD_H
#define HAL_LCD_H

#if defined(BOARD_arduinoMega)
	#include "hal/board/arduinoMega/hal_lcd_AMC2004_impl.h"
	#define HAL_LCD
#endif

#if !defined(HAL_LCD)
    #error "No hal implementation for lcd on selected hardware target."
#endif
#endif
