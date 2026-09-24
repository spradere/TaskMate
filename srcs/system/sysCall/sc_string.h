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

#include "interfaces/error_catalog.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * String macro redirection
 * ========================================================================== */

#if defined(HAL_STRING) && defined(HAL_STRING_INROM) && defined(HAL_STRING_RAM) && \
	defined(HAL_STRING_ROM)
	#define TM_STR(string) HAL_STRING(string)

	#define TM_STR_RAM(string) HAL_STRING_RAM(string)
	#define TM_STR_RON(string) HAL_STRING_ROM(string)

	#define TM_STR_NEW(name, txt) HAL_STRING_INROM(name, txt)
#else
	#define TM_STR(string) (string)

	#define TM_STR_RAM(string) (string)
	#define TM_STR_RON(string) (string)

	#define TM_STR_NEW(name, txt) char name[] = (txt);
#endif

/* ============================================================================
 * Public API
 * ========================================================================== */

err_codes_t sc_consoleWriteByte(uint8_t data);
err_codes_t sc_consoleFlush(void);

uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index);

int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size);
void sc_stringCopy(char *dest, tm_string_t src, uint8_t size);

#endif // SYSCALL_SC_STRING_H
