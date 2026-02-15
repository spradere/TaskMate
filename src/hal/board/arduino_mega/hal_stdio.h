/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_stdio.h
 * @brief board stdio header
 *
 */

#ifndef HAL_STDIO_H
#define HAL_STDIO_H

void hal_stdio_putChar(char ch);
char hal_sdtio_getChar(void);

#endif
