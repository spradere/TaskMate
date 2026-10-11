/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_ucontext_string.c
 * @brief Compile checks for ucontext byte-oriented string storage.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <stdint.h>

#define TM_LIBC_TASKMATE 0
#define TM_LIBC_CSTD 1
#include "hal/host/ucontext/ucontext_string_macro.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

_Static_assert(
	__builtin_types_compatible_p(tm_string_t, char), "host strings must use char");

HAL_STRING_INROM(rom_text, "rom");

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

uint8_t testUcontextString(void)
{
	const char text[] = "ram";
	const uint8_t bytes[] = {0x80U, 0U};
	const tm_string_t *literal = HAL_STRING_ROM("literal");
	const tm_string_t *ram_text = HAL_STRING_RAM(text);
	const tm_string_t *ram_bytes = HAL_STRING_RAM((const char *)bytes);

	return (uint8_t)(HAL_STRING_ROMGETBYTE(rom_text) +
		HAL_STRING_ROMGETBYTE(literal) + ram_text[0U] + ram_bytes[0U]);
}
