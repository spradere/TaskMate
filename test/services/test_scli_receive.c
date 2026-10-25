/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_scli_receive.c
 * @brief Check SCLI line assembly across USART reads.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define TM_LIBC_TASKMATE 0
#define TM_LIBC_CSTD 1
#include "system/services/scli.c"

/* -----------------------------------------------
 * Test state
 * ---------------------------------------------*/

static const char *rx_text;
static size_t rx_length;
static size_t rx_position;
static uint8_t stack_call_count;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

err_codes_t sc_usartRead(uint8_t *data)
{
	if( rx_position >= rx_length ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }
	*data = (uint8_t)rx_text[rx_position++];
	return ERR_NO_ERROR;
}

bool stackCommand(uint8_t argc, char *argv[])
{
	assert(argc == 2U);
	assert(strcmp(argv[0U], "stack") == 0);
	assert(strcmp(argv[1U], "depth") == 0);
	stack_call_count++;
	return true;
}

bool dateCommand(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;
	assert(false);
	return false;
}

bool driverCommand(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;
	assert(false);
	return false;
}

bool i2cCommand(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;
	assert(false);
	return false;
}

bool threadCommand(uint8_t argc, char *argv[])
{
	(void)argc;
	(void)argv;
	assert(false);
	return false;
}

static void readChunk(const char *text)
{
	rx_text = text;
	rx_length = strlen(text);
	rx_position = 0U;
	do
	{
		assert(scliRead() == ERR_NO_ERROR);
	} while( rx_position < rx_length );
}

int main(void)
{
	readChunk("sta");
	assert(stack_call_count == 0U);
	assert(scli_line_length == 3U);
	readChunk("ck depth\r");
	assert(stack_call_count == 1U);
	assert(scli_line_length == 0U);

	readChunk("\nstack depth\r\nstack\tdepth\n");
	assert(stack_call_count == 3U);

	readChunk("stack depth");
	assert(stack_call_count == 3U);
	readChunk("\n");
	assert(stack_call_count == 4U);

	char long_line[SCLI_LINE_SIZE + sizeof("\n")];
	memset(long_line, ' ', SCLI_LINE_SIZE);
	memcpy(long_line, "stack depth", sizeof("stack depth") - 1U);
	long_line[SCLI_LINE_SIZE] = '\n';
	long_line[SCLI_LINE_SIZE + 1U] = 0U;
	readChunk(long_line);
	assert(stack_call_count == 4U);
	assert(scli_line_length == 0U);
	readChunk("stack depth\n");
	assert(stack_call_count == 5U);

	return 0;
}
