/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_sc_errors.c
 * @brief Exercise error lookup at the generated catalogue capacity boundary.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdlib.h>

#include "interfaces/tm_string.h"

/* Keep generated messages in host memory while exercising the syscall source. */
#define HAL_STRING(string) ((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_ROM})
#define HAL_STRING_RAM(string) \
	((tm_string_t){.text = (const uint8_t *)(string), .storage = TM_MEM_RAM})
#define HAL_STRING_ROM(string) HAL_STRING(string)
#define HAL_STRING_INROM(name, txt) \
	static const tm_string_t name = {.text = (const uint8_t *)(txt), .storage = TM_MEM_ROM}

#include "system/sysCall/sc_errors.c"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

_Noreturn void hal_halt(void) { abort(); }

int main(void)
{
	assert(err_getMessage(ERR_BOUND_000) != NULL);
	assert(err_getMessage((err_codes_t)(ERROR_COUNT - 1U)) != NULL);
	assert(err_getMessage(ERROR_COUNT) == NULL);
	return 0;
}
