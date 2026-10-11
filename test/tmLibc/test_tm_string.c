/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_tm_string.c
 * @brief Verify standard bounded string behavior with TaskMate descriptors.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "tmLibc/tm_string.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define TEST_STRING_COPY_SIZE 5U
#define TEST_STRING_SHORT_SIZE 2U
#define TEST_STRING_HIGH_BYTE 0xFFU

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

uint8_t sc_stringGetByte(const tm_string_t *string, size_t index)
{
	return string->text[index];
}

int main(void)
{
	const uint8_t text[] = {'a', 'b', 0U};
	const uint8_t higher[] = {'a', 'c', 0U};
	const uint8_t high_byte[] = {TEST_STRING_HIGH_BYTE, 0U};
	const uint8_t low_byte[] = {1U, 0U};
	tm_string_t source = {.text = text, .storage = TM_MEM_RAM};
	tm_string_t greater = {.text = higher, .storage = TM_MEM_ROM};
	tm_string_t high = {.text = high_byte, .storage = TM_MEM_RAM};
	tm_string_t low = {.text = low_byte, .storage = TM_MEM_RAM};
	char padded[TEST_STRING_COPY_SIZE];
	char truncated[TEST_STRING_SHORT_SIZE];

	assert(strncmp(source, greater, 0U) == 0);
	assert(strncmp(source, greater, 1U) == 0);
	assert(strncmp(source, greater, TEST_STRING_COPY_SIZE) < 0);
	assert(strncmp(greater, source, TEST_STRING_COPY_SIZE) > 0);
	assert(strncmp(source, source, TEST_STRING_COPY_SIZE) == 0);
	assert(strncmp(high, low, 1U) > 0);

	assert(strncpy(padded, source, sizeof(padded)) == padded);
	assert(padded[0U] == 'a');
	assert(padded[1U] == 'b');
	assert(padded[TEST_STRING_SHORT_SIZE] == 0);
	assert(padded[TEST_STRING_COPY_SIZE - 1U] == 0);
	assert(strncpy(truncated, source, sizeof(truncated)) == truncated);
	assert(truncated[0U] == 'a');
	assert(truncated[1U] == 'b');
	return 0;
}
