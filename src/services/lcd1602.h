/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file lcd1602.h
 * @brief header of lcd display 1602 2x16
 *
 * @todo Nothing
 */

#ifndef LCD1602_H
#define LCD1602_H

void lcd1602(void);

void lcd1602Init(void);
void lcd1602Clear(void);
void lcd1602SetCursor(uint8_t row, uint8_t col);
void lcd1602WriteString(const char *str);

#endif
