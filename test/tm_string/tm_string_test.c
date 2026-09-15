/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_string_test.c
 * @brief Host tests for TaskMate string primitives and formatting.
 */

#include <assert.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

/* Keep the host test independent from AVR headers and hardware registers. */
#define SYSCALL_SC_STRING_H
#define INTERFACES_DRV_USART_H
#define INTERFACES_TM_DEFINE_H
#define TM_LIBC_TM_SNPRINTF_H

#define TM_STRING_SIZE_MAX 255

typedef enum
{
	TM_MEM_RAM,
	TM_MEM_ROM
} tm_string_storage_t;

typedef struct
{
	const char *text;
	const tm_string_storage_t storage;
} tm_string_t;

typedef uint8_t err_codes_t;
typedef uint8_t hal_driver_state_t;
typedef uint8_t hal_driver_control_t;
typedef struct
{
	err_codes_t error;
} hal_driver_control_data_t;

#define DRV_STATE_RUNNING 1
#define DRV_CTRL_GETLASTERROR 1
#define ERR_NO_ERROR 0
#define ERR_HAL_USART_TX_BUFFER_FULL 1
#define pgm_read_byte(address) (*(const uint8_t *)(address))

err_codes_t sc_consoleWriteByte(uint8_t data);
err_codes_t sc_consoleFlush(void);
tm_string_t sc_stringFromBuffer(const char *text);
uint8_t sc_stringGetByte(const tm_string_t *string, uint8_t index);
int sc_stringCompare(tm_string_t left, tm_string_t right, uint8_t size);
void sc_stringCopy(char *dest, tm_string_t src, uint8_t size);
int tm_printf(tm_string_t format, ...);
int tm_vprintf(tm_string_t format, va_list args);
int tm_snprintf(char *ptr, uint8_t size, tm_string_t format, ...);
int tm_vsnprintf(char *ptr, uint8_t size, tm_string_t format, va_list args);

static uint16_t console_byte_count;
static uint16_t console_flush_count;

hal_driver_state_t hal_usartWriteByte(uint8_t data)
{
	(void)data;
	console_byte_count++;
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartSendTXBuffer(void)
{
	console_flush_count++;
	return DRV_STATE_RUNNING;
}

hal_driver_state_t hal_usartControl(hal_driver_control_t command, hal_driver_control_data_t *data)
{
	(void)command;
	data->error = ERR_NO_ERROR;
	return DRV_STATE_RUNNING;
}

#include "../../srcs/system/sysCall/sc_string.c"
#include "../../srcs/tmLibc/tm_snprintf.c"

static tm_string_t stringFrom(const char *text)
{
	return (tm_string_t){.text = text, .storage = TM_MEM_RAM};
}

static tm_string_t stringFromRom(const char *text)
{
	return (tm_string_t){.text = text, .storage = TM_MEM_ROM};
}

static void testGetByte(void)
{
	const char high_byte[] = {(char)0x80, 0};
	const char bounded[TM_STRING_SIZE_MAX + 1] = {[0] = 'a', [254] = 'z', [255] = 'x'};
	const tm_string_t null_text = stringFrom(NULL);
	const tm_string_t invalid_storage = {.text = "invalid", .storage = (tm_string_storage_t)2};
	const tm_string_t high_string = stringFrom(high_byte);
	const tm_string_t bounded_string = stringFrom(bounded);

	assert(sc_stringGetByte(NULL, 0) == 0);
	assert(sc_stringGetByte(&null_text, 0) == 0);
	assert(sc_stringGetByte(&invalid_storage, 0) == 0);
	assert(sc_stringGetByte(&high_string, 0) == 0x80);
	assert(sc_stringGetByte(&bounded_string, 254) == (uint8_t)'z');
	assert(sc_stringGetByte(&bounded_string, TM_STRING_SIZE_MAX) == 0);
}

static void testCompareAndCopy(void)
{
	const tm_string_t empty = stringFrom(NULL);
	const tm_string_t text = stringFrom("abc");
	const tm_string_t rom_text = stringFromRom("abc");
	const char high_left[] = {(char)0x80, 0};
	const char high_right[] = {(char)0x7f, 0};
	const tm_string_t invalid_storage = {.text = "invalid", .storage = (tm_string_storage_t)2};
	char long_source[TM_STRING_SIZE_MAX];
	char long_destination[TM_STRING_SIZE_MAX];
	char destination[4] = {'x', 'x', 'x', 'x'};

	assert(sc_stringCompare(empty, stringFrom(NULL), TM_STRING_SIZE_MAX) == 0);
	assert(sc_stringCompare(empty, text, TM_STRING_SIZE_MAX) < 0);
	assert(sc_stringCompare(text, stringFrom("abd"), TM_STRING_SIZE_MAX) < 0);
	assert(sc_stringCompare(text, stringFrom("abc"), 0) == 0);
	assert(sc_stringCompare(text, rom_text, TM_STRING_SIZE_MAX) == 0);
	assert(sc_stringCompare(rom_text, stringFromRom("abc"), TM_STRING_SIZE_MAX) == 0);
	assert(sc_stringCompare(stringFrom(high_left), stringFrom(high_right), 1) > 0);
	assert(sc_stringCompare(invalid_storage, stringFrom(""), TM_STRING_SIZE_MAX) == 0);

	sc_stringCopy(NULL, text, sizeof(destination));
	sc_stringCopy(destination, empty, sizeof(destination));
	assert(destination[0] == 0);
	destination[0] = 'x';
	sc_stringCopy(destination, text, 0);
	assert(destination[0] == 'x');
	sc_stringCopy(destination, text, 1);
	assert(destination[0] == 0);
	sc_stringCopy(destination, rom_text, sizeof(destination));
	assert(strcmp(destination, "abc") == 0);
	sc_stringCopy(destination, invalid_storage, sizeof(destination));
	assert(destination[0] == 0);
	memset(long_source, (int)'x', sizeof(long_source));
	sc_stringCopy(long_destination, stringFrom(long_source), sizeof(long_destination));
	assert(long_destination[TM_STRING_SIZE_MAX - 2] == 'x');
	assert(long_destination[TM_STRING_SIZE_MAX - 1] == 0);
}

static void testFormat(void)
{
	char untouched = 'x';
	char one_byte = 'x';
	char two_bytes[2] = {'x', 'x'};
	char output[8];
	const tm_string_t null_string = stringFrom(NULL);

	assert(tm_snprintf(&untouched, 0, stringFrom("a")) == 0);
	assert(untouched == 'x');
	assert(tm_snprintf(&one_byte, 1, stringFrom("a")) == 0);
	assert(one_byte == 0);
	assert(tm_snprintf(two_bytes, sizeof(two_bytes), stringFrom("ab")) == 1);
	assert(strcmp(two_bytes, "a") == 0);
	assert(tm_snprintf(output, sizeof(output), stringFrom("<%s>"), NULL) == 2);
	assert(strcmp(output, "<>") == 0);
	assert(tm_snprintf(output, sizeof(output), stringFrom("<%s>"), &null_string) == 2);
	assert(strcmp(output, "<>") == 0);
	output[0] = (char)0x7f;
	assert(tm_snprintf(output, sizeof(output), stringFrom("%0:i"), 1) == 0);
	assert(output[0] == 0);

	console_byte_count = 0;
	console_flush_count = 0;
	assert(tm_printf(stringFrom("a\n")) == 0);
	assert(console_byte_count == 2);
	assert(console_flush_count == 1);
}

int main(void)
{
	testGetByte();
	testCompareAndCopy();
	testFormat();
	return 0;
}
