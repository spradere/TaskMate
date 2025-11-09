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

#ifndef LCDAMC2004_H
#define LCDAMC2004_H

void lcdAMC2004Init(void);
void lcdAMC2004Start(void);
void lcdAMC2004Stop(void);

void lcdAMC2004Clear(void);
void lcdAMC2004SendCommand(uint8_t command);
void lcdAMC2004SetCursor(uint8_t row, uint8_t col);
void lcdAMC2004WriteString(const char *str);

#endif
