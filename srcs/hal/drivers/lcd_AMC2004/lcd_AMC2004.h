/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file lcd_AMC2004.h
 * @brief lcd amc2004 header declarations.
 *
 */

#ifndef LCD_AMC2004_LCD_AMC2004_H
#define LCD_AMC2004_LCD_AMC2004_H

#include <stdint.h>

uint8_t hal_lcdControl(uint8_t cmd, uint8_t val);
void hal_lcdClear(void);
void hal_lcdSetCursor(uint8_t row, uint8_t col);
void hal_lcdWriteString(const char *str);

#endif // LCD_AMC2004_LCD_AMC2004_H
