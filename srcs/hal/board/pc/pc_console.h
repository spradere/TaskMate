/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file pc_console.h
 * @brief PC ncurses console declarations.
 */

#ifndef PC_PC_CONSOLE_H
#define PC_PC_CONSOLE_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * Private board API
 * ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

bool pc_consoleInit(void);
void pc_consoleShutdown(void);
void pc_consoleWriteByte(uint8_t data);
void pc_consoleFlush(void);
bool pc_consoleReadByte(uint8_t *data);
void pc_consolePollInput(void);

#ifdef __cplusplus
}
#endif

#endif // PC_PC_CONSOLE_H
