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
 * @file lcdAMC2004.h
 * @brief header of lcd display
 *
 * @todo Nothing
 */

#ifndef LCDAMC2004_H
#define LCDAMC2004_H

void lcdAMC2004SetStatus(uint8_t status);
uint8_t lcdAMC2004GetStatus(void);

void lcdAMC2004(void);

void lcdAMC2004Init(void);
void lcdAMC2004Clear(void);
void lcdAMC2004SetCursor(uint8_t row, uint8_t col);
void lcdAMC2004WriteString(const char *str);

#endif
