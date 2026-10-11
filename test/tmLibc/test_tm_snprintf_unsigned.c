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
#include <stdbool.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#include "system/sysCall/sc_string.h"
#include "tmLibc/stdio/tm_snprintf.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TEST_SNPRINTF_BUFFER_SIZE 64U
#define TEST_SNPRINTF_TRUNCATED_SIZE 4U
#define TEST_SNPRINTF_TEXT_LENGTH 6
#define TEST_SNPRINTF_STRING_LENGTH 5
#define TEST_SNPRINTF_ABOVE_UINT16 ((unsigned int)UINT16_MAX + 1U)

_Static_assert(UINT_MAX == UINT32_MAX, "host test requires a 32-bit unsigned int");

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void testUnsigned(const char *format_text, unsigned int value, const char *expected);
static bool textEquals(const char *left, const char *right);

static unsigned int test_console_bytes;
static unsigned int test_console_flushes;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Syscall dependencies
 * ---------------------------------------------*/

err_codes_t sc_consoleWriteByte(uint8_t data)
{
	(void)data;
	test_console_bytes++;
	return ERR_NO_ERROR;
}

err_codes_t sc_consoleFlush(void)
{
	test_console_flushes++;
	return ERR_NO_ERROR;
}

uint8_t sc_stringGetByte(const tm_string_t *string, size_t index)
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

	int count = snprintf(buffer, TEST_SNPRINTF_BUFFER_SIZE, format, value);
	size_t length = 0U;
	while( expected[length] != 0 ) { length++; }
	assert(count == (int)length);
	assert(textEquals(buffer, expected));
}

static bool textEquals(const char *left, const char *right)
{
	size_t index = 0U;
	while( left[index] == right[index] )
	{
		if( left[index] == 0 ) { return true; }
		index++;
	}
	return false;
}

int main(void)
{
	char short_buffer[TEST_SNPRINTF_TRUNCATED_SIZE];
	char full_buffer[TEST_SNPRINTF_BUFFER_SIZE];
	tm_string_t format = {.text = (const uint8_t *)"abcdef", .storage = TM_MEM_RAM};
	tm_string_t string_format = {
		.text = (const uint8_t *)"X%s%%\n", .storage = TM_MEM_RAM};
	tm_string_t word = {.text = (const uint8_t *)"ab", .storage = TM_MEM_RAM};

	testUnsigned("%u", UINT16_MAX, "65535");
	testUnsigned("%02u", 1U, "01");
	testUnsigned("%u", TEST_SNPRINTF_ABOVE_UINT16, "65536");
	testUnsigned("%x", TEST_SNPRINTF_ABOVE_UINT16, "10000");
	testUnsigned("%b", TEST_SNPRINTF_ABOVE_UINT16, "10000000000000000");

	testUnsigned("%u", UINT_MAX, "4294967295");
	testUnsigned("%b", UINT_MAX, "11111111111111111111111111111111");
	assert(snprintf(short_buffer, sizeof(short_buffer), format) == TEST_SNPRINTF_TEXT_LENGTH);
	assert(textEquals(short_buffer, "abc"));
	assert(snprintf(NULL, 0U, format) == TEST_SNPRINTF_TEXT_LENGTH);
	assert(test_console_bytes == 0U);
	assert(snprintf(full_buffer, sizeof(full_buffer), string_format, &word) ==
		TEST_SNPRINTF_STRING_LENGTH);
	assert(textEquals(full_buffer, "Xab%\n"));
	assert(printf(string_format, &word) == TEST_SNPRINTF_STRING_LENGTH);
	assert(test_console_bytes == TEST_SNPRINTF_STRING_LENGTH);
	assert(test_console_flushes == 1U);
	return 0;
}
