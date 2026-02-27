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
 * @file hal_lcd_AMC2004.h
 * @brief header hal lcd implementation
 *
 */

// @hal_user

#ifndef HAL_LCD_H
#define HAL_LCD_H

#include <stdint.h>

void hal_lcdInit(void);
void hal_lcdStart(void);
void hal_lcdStop(void);
void hal_lcdClear(void);
void hal_lcdSetCursor(uint8_t row, uint8_t col);
void hal_lcdWriteString(const char *str);

#endif
