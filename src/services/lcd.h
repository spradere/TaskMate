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
 * @file lcd.h
 * @brief header of lcd display
 *
 * @todo Nothing
 */

#ifndef LCD_H
#define LCD_H

void lcdSetStatus(uint8_t status);
uint8_t lcdGetStatus(void);

void lcd(void);

void lcdInit(void);
void lcdClear(void);
void lcdSetCursor(uint8_t row, uint8_t col);
void lcdWriteString(const char * str);

#endif
