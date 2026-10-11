/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file ucontext_string_macro.h
 * @brief ucontext host string macro declarations.
 */

#ifndef UCONTEXT_UCONTEXT_STRING_MACRO_H
#define UCONTEXT_UCONTEXT_STRING_MACRO_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>
#include <string.h>

#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @name Host string storage
 * @brief Create string descriptors backed by host memory.
 * @{
 */
#define HAL_STRING_INROM(name, txt) static const char name[] = (txt)

#define HAL_STRING_ROM(string) (string)
#define HAL_STRING_RAM(string) (string)
#define HAL_STRING_ROMGETBYTE(ptr) (*(const uint8_t *)(ptr))
#define HAL_STRING_COMPARE(left, right, size) strncmp((left), (right), (size))

#define HAL_STRING(string) HAL_STRING_ROM(string)
/** @} */

#endif // UCONTEXT_UCONTEXT_STRING_MACRO_H
