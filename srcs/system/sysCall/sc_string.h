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
#include <stddef.h>

#include "interfaces/error_catalog.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * String macro redirection
 * ========================================================================== */

/**
 * @name TaskMate string constructors
 * @brief Select RAM or ROM storage through the configured string backend.
 * @{
 */
#if TM_LIBC_TASKMATE
	#define TM_STR(string) HAL_STRING(string)

	#define TM_STR_RAM(string) HAL_STRING_RAM(string)
	#define TM_STR_ROM(string) HAL_STRING_ROM(string)
	#define TM_ROM(string) HAL_STRING_ROM(string)

	#define TM_STR_NEW(name, txt) HAL_STRING_INROM(name, txt)
	#define TM_STR_REF(name) (&(name))
	#define TM_STR_ARG(name) (&(name))
	#define TM_STR_DATA(name) ((name).text)
	#define TM_STR_VALUE(name) (*(name))
#else
	#define TM_STR(string) (string)

	#define TM_STR_RAM(string) (string)
	#define TM_STR_ROM(string) (string)
	#define TM_ROM(string) (string)

	#define TM_STR_NEW(name, txt) static const char name[] = (txt)
	#define TM_STR_REF(name) (name)
	#define TM_STR_ARG(name) (name)
	#define TM_STR_DATA(name) (name)
	#define TM_STR_VALUE(name) (name)
#endif
/** @} */

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Send one byte to the console.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_consoleWriteByte(uint8_t data);
/**
 * @brief Flush pending console output.
 * @return ERR_NO_ERROR on success; an error code otherwise.
 */
err_codes_t sc_consoleFlush(void);

/**
 * @brief Read a byte from RAM or ROM string storage.
 * @param string Storage-aware string descriptor.
 * @param index Byte offset in the string.
 * @return Byte at the requested offset.
 */
uint8_t sc_stringGetByte(const tm_string_t *string, size_t index);

#endif // SYSCALL_SC_STRING_H
