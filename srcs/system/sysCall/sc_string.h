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

#include "hal/public/hal_string.h"
#include "interfaces/error_catalog.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * Write one byte to the system console without requiring scheduler services.
 *
 * This call is valid during early boot and must not be called from an ISR.
 */
err_codes_t sc_consoleWriteByte(uint8_t data);

/**
 * Flush the system console without requiring scheduler services.
 *
 * This call is valid during early boot and must not be called from an ISR.
 */
err_codes_t sc_consoleFlush(void);

/** Create a RAM string descriptor. A null text pointer represents an empty string. */
tm_string_t sc_stringFromBuffer(const char *text);

/**
 * Read one byte without exceeding the string contract.
 *
 * Null descriptor or text pointers, unknown storage, and indexes at or above
 * TM_STRING_SIZE_MAX produce the terminating NUL byte.
 */
uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index);

/** Compare at most size bytes. Null text and unknown storage represent empty strings. */
int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size);

/** Copy and NUL-terminate when dest is non-null and size is non-zero. */
void sc_stringCopy(char *dest, tm_string_t src, uint8_t size);

#endif // SYSCALL_SC_STRING_H
