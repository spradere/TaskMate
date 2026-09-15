/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_string.h
 * @brief String syscall declarations.
 *
 */

#ifndef SYSCALL_SC_STRING_H
#define SYSCALL_SC_STRING_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

#include "hal/public/hal_define.h" // get string macros from arch
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

void sc_stdioPutChar(char ch);
tm_string_t sc_stringFromBuffer(const char *text);
uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index);
int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size);
void sc_stringCopy(char *dest, tm_string_t src, uint8_t size);

#endif // SYSCALL_SC_STRING_H
