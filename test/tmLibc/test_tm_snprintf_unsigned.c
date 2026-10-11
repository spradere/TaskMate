/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_tm_snprintf_unsigned.c
 * @brief Verify unsigned formatting across the 16-bit boundary.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "system/sysCall/sc_string.h"
#include "tmLibc/stdio/tm_snprintf.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TEST_SNPRINTF_BUFFER_SIZE 64U
#define TEST_SNPRINTF_ABOVE_UINT16 ((unsigned int)UINT16_MAX + 1U)

_Static_assert(UINT_MAX == UINT32_MAX, "host test requires a 32-bit unsigned int");

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void testUnsigned(const char *format_text, unsigned int value, const char *expected);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Syscall dependencies
 * ---------------------------------------------*/

err_codes_t sc_consoleWriteByte(uint8_t data)
{
	(void)data;
	return (err_codes_t)0U;
}

err_codes_t sc_consoleFlush(void) { return (err_codes_t)0U; }

uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index)
{
	return string->text[index];
}

/* -----------------------------------------------
 * Test cases
 * ---------------------------------------------*/

static void testUnsigned(const char *format_text, unsigned int value, const char *expected)
{
	char buffer[TEST_SNPRINTF_BUFFER_SIZE];
	tm_string_t format = {.text = (const uint8_t *)format_text, .storage = TM_MEM_RAM};

	int count = tm_snprintf(buffer, TEST_SNPRINTF_BUFFER_SIZE, format, value);
	assert(count == (int)strlen(expected));
	assert(strcmp(buffer, expected) == 0);
}

int main(void)
{
	char expected[TEST_SNPRINTF_BUFFER_SIZE];

	testUnsigned("%u", UINT16_MAX, "65535");
	testUnsigned("%u", TEST_SNPRINTF_ABOVE_UINT16, "65536");
	testUnsigned("%x", TEST_SNPRINTF_ABOVE_UINT16, "10000");
	testUnsigned("%b", TEST_SNPRINTF_ABOVE_UINT16, "10000000000000000");

	(void)snprintf(expected, sizeof(expected), "%u", UINT_MAX);
	testUnsigned("%u", UINT_MAX, expected);
	testUnsigned("%b", UINT_MAX, "11111111111111111111111111111111");
	return 0;
}
