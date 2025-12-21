/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_lcd.h
 * @brief header hal lcd implemetation
 *
 */

#include <stdint.h>

void hal_lcdInit(void);
void hal_lcdStart(void);
void hal_lcdStop(void);
void hal_lcdClear(void);
void hal_lcdSetCursor(uint8_t row, uint8_t col);
void hal_lcdWriteString(const char *str);
