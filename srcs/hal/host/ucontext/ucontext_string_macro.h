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

#include "interfaces/tm_macros.h"
#include "interfaces/tm_string.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

#define HAL_STRING_INROM(name, txt)                     \
	static const char TM_UNIQUE_NAME(name)[] = (txt);     \
	static const tm_string_t(name) = {                    \
		.text = TM_UNIQUE_NAME(name), .storage = TM_MEM_ROM}

#define HAL_STRING_ROM(string) ((tm_string_t){.text = (string), .storage = TM_MEM_ROM})
#define HAL_STRING_RAM(string) ((tm_string_t){.text = (string), .storage = TM_MEM_RAM})
#define HAL_STRING_ROMGETBYTE(ptr) (*(const uint8_t *)(ptr))

#define HAL_STRING(string) HAL_STRING_ROM(string)

#endif // UCONTEXT_UCONTEXT_STRING_MACRO_H
