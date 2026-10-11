/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tm_snprintf.c
 * @brief tm snprintf implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "tm_snprintf.h"

#if TM_LIBC_TASKMATE
#include <limits.h>
#include <stdbool.h>

#include "interfaces/tm_define.h"
#include "system/sysCall/sc_string.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void baseConvert(unsigned int value, uint8_t base);
static void putChar(char ch);
static int formatString(char *ptr, size_t size, bool console, tm_string_t format, va_list args);

/* -----------------------------------------------
 * Formatter state
 * ---------------------------------------------*/

#define SNPRINTF_BUFF_TEMP_SIZE 32
#define SNPRINTF_BASE_BINARY 2U
#define SNPRINTF_BASE_DECIMAL 10U
#define SNPRINTF_BASE_HEXADECIMAL 16U
#define SNPRINTF_PADDING_MAX 9U
#define SNPRINTF_ERROR (-1)

typedef struct
{
	char *ptr;
	size_t size;
	size_t index;
	uint8_t padding;
	bool console;
	bool failed;
} tm_snprintf_buffer_t;

static tm_snprintf_buffer_t tm_snprintf_buffer;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

static void baseConvert(unsigned int value, uint8_t base)
{
	const char digits[] = "0123456789abcdef";

	char tmp[SNPRINTF_BUFF_TEMP_SIZE];
	uint8_t pos = 0U;

	// Convert in reverse order
	if( value == 0U ) { tmp[pos++] = '0'; }
	else
	{
		while( (value != 0U) && (pos < SNPRINTF_BUFF_TEMP_SIZE) )
		{
			uint8_t data = (uint8_t)(value % base);
			value /= base;
			tmp[pos++] = digits[data];
		}
	}

	while( pos < tm_snprintf_buffer.padding ) { tmp[pos++] = '0'; }

	// Reverse order
	while( pos > 0U ) { putChar(tmp[--pos]); }
}

int snprintf(char *ptr, size_t size, const tm_string_t format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = vsnprintf(ptr, size, format, args);
	va_end(args);
	return ret;
}

int printf(const tm_string_t format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = vprintf(format, args);
	va_end(args);
	return ret;
}

int vprintf(const tm_string_t format, va_list args)
{
	return formatString(NULL, 0U, true, format, args);
}

static void putChar(char ch)
{
	if( tm_snprintf_buffer.console )
	{
		if( sc_consoleWriteByte((uint8_t)ch) != ERR_NO_ERROR ) { tm_snprintf_buffer.failed = true; }
		if( (ch == '\n') && (sc_consoleFlush() != ERR_NO_ERROR) )
		{
			tm_snprintf_buffer.failed = true;
		}
	}
	else if( (tm_snprintf_buffer.ptr != NULL) && (tm_snprintf_buffer.size > 0U) &&
		(tm_snprintf_buffer.index < (tm_snprintf_buffer.size - 1U)) )
	{
		tm_snprintf_buffer.ptr[tm_snprintf_buffer.index] = ch;
	}
	tm_snprintf_buffer.index++;
}

/*
 * Format through shared static state. Calls must not overlap, including calls from an ISR
 * or another thread. The stream entry points send bytes to the console.
 */
int vsnprintf(char *ptr, size_t size, const tm_string_t format, va_list args)
{
	return formatString(ptr, size, false, format, args);
}

static int formatString(char *ptr, size_t size, bool console, const tm_string_t format, va_list args)
{

	// Store variables
	tm_snprintf_buffer.ptr = ptr;
	tm_snprintf_buffer.size = size;
	tm_snprintf_buffer.index = 0U;
	tm_snprintf_buffer.padding = 0U;
	tm_snprintf_buffer.console = console;
	tm_snprintf_buffer.failed = false;

	size_t format_index = 0U;
	char format_c = (char)sc_stringGetByte(&format, format_index++);

	while( format_c )
	{
		// Consume a conversion marker and its optional one-digit zero width.
		if( format_c == '%' )
		{
			format_c = (char)sc_stringGetByte(&format, format_index++);

			if( (format_c) == '0' )
			{
				format_c = (char)sc_stringGetByte(&format, format_index++);
				tm_snprintf_buffer.padding = (uint8_t)(format_c - '0');
				if( tm_snprintf_buffer.padding > SNPRINTF_PADDING_MAX ) { goto exit; }
				format_c = (char)sc_stringGetByte(&format, format_index++);
			}

			// Each supported conversion emits through the same character sink.
			switch( format_c )
			{

				case 'c':
				{
					int cc = va_arg(args, int);
					putChar((char)cc);
					break;
				}

				case 's':
				{
					const tm_string_t *str = va_arg(args, const tm_string_t *);
					size_t str_index = 0U;
					char str_c = (char)sc_stringGetByte(str, str_index++);
					while( str_c != 0U )
					{
						putChar(str_c);
						str_c = (char)sc_stringGetByte(str, str_index++);
					}
					break;
				}

				// Numeric conversions share base conversion and padding.
				case 'u':
				case 'x':
				case 'b':
				{
					unsigned int value = va_arg(args, unsigned int);
					uint8_t base;

					switch( format_c )
					{
						case 'u':
							base = SNPRINTF_BASE_DECIMAL;
							break;
						case 'x':
							base = SNPRINTF_BASE_HEXADECIMAL;
							break;
						case 'b':
							base = SNPRINTF_BASE_BINARY;
							break;
						default:
							base = SNPRINTF_BASE_DECIMAL;
							break;
					}
					baseConvert(value, base);
					break;
				}

				case '%':
					putChar('%');
					break;
				default:
					break;
			}
			tm_snprintf_buffer.padding = 0U;
		}
		else { putChar(format_c); }
		format_c = (char)sc_stringGetByte(&format, format_index++);
	}

exit:

	if( (ptr != NULL) && (size > 0U) )
	{
		size_t end = tm_snprintf_buffer.index;
		if( end >= size ) { end = size - 1U; }
		ptr[end] = 0U;
	}
	if( tm_snprintf_buffer.failed || (tm_snprintf_buffer.index > INT_MAX) )
	{
		return SNPRINTF_ERROR;
	}
	return (int)tm_snprintf_buffer.index;
}
#endif
