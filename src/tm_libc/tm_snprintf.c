/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file tm_snprintf.c
 * @brief implementation of libc formatted print in string
 *
 */

#include "tm_libc/tm_snprintf.h"

#include <stddef.h>

#include "hal/auto_hal_tmlibc.h"

static void baseConvert(uint16_t value, uint8_t base);
static void tm_putChar(char ch);

#define SNPRINFT_BUFF_TEMP_SIZE 32

// structure for buffer data
struct buff_t
{
	char *ptr;
	uint8_t size;
	uint8_t index;
	uint8_t padding;
} buff;

// reverse order base converter
static void baseConvert(uint16_t value, uint8_t base)
{
	const char digits[] = "0123456789abcdef";

	char tmp[SNPRINFT_BUFF_TEMP_SIZE];
	uint8_t pos = 0;

	// reverse order convert
	if( value == 0 ) {tmp[pos++] = '0';}
	else
	{
		while( (value != 0) && (pos < SNPRINFT_BUFF_TEMP_SIZE) )
		{
			uint16_t data = (value % base);
			value /= base;
			tmp[pos++] = digits[data];
		}
	}

	while( pos < buff.padding ) { tmp[pos++] = '0'; }

	// reverse order
	while( pos > 0 )
	{
		tm_putChar(tmp[--pos]);
	}
}

int tm_snprintf(char *ptr, uint8_t size, const tm_string_t format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = tm_vsnprintf(ptr, size, format, args);
	va_end(args);
	return ret;
}

int tm_printf(const tm_string_t format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = tm_vprintf(format, args);
	va_end(args);
	return ret;
}

int tm_vprintf(const tm_string_t format, va_list args)
{
	return tm_vsnprintf(NULL, 0, format, args);
}

static void tm_putChar(char ch)
{
	if( buff.ptr != NULL)
	{
		if( (buff.index + 1) < buff.size ) { buff.ptr[buff.index++] = ch; }
	}

	if(buff.ptr == NULL)
	{
		hal_stdio_putChar(ch);
	}
}

int tm_vsnprintf(char *ptr, uint8_t size, const tm_string_t format, va_list args)
{
	// store variables
	buff.ptr = ptr;
	buff.size = size;
	buff.index = 0;
	buff.padding = 0;

	uint8_t format_index= 0;
	char format_c = hal_string_getChar(&format, format_index++);

	while( format_c )
	{
		if( format_c == '%' )
		{
			format_c = hal_string_getChar(&format, format_index++);

			if( (format_c) == '0' )
			{
				format_c = hal_string_getChar(&format, format_index++);
				buff.padding = (uint8_t)(format_c - 48); // atoi
				if( buff.padding > 9 ){ return 0; }
				format_c = hal_string_getChar(&format, format_index++);
			}

			switch( format_c )
			{

				case 'c':
				{
					int cc = va_arg(args, int);
					tm_putChar((char)cc);
					break;
				}
				/*case 's':
				{
					const char *s = va_arg(args, char *);
					while( *s )
					{
						tm_putChar(*s);
						s++;
					}
					break;
				}*/
				case 's':
				{
					const tm_string_t str = va_arg(args, tm_string_t );
					uint8_t str_index = 0;
					char str_c = hal_string_getChar(&str, str_index++);
					while( str_c != 0 )
					{
						tm_putChar(str_c);
						str_c = hal_string_getChar(&str, str_index++);
					}
					break;
				}


				case 'i':
				case 'x':
				case 'b':
				{
					uint16_t value = va_arg(args, uint16_t);
					uint8_t base;

					switch( format_c )
					{
						case 'i':
							base = 10;
							break;
						case 'x':
							base = 16;
							break;
						case 'b':
							base = 2;
							break;
						default:
							base = 10;
							break;
					}
					baseConvert(value, base);
					buff.padding = 0;
					break;
				}

				case '%':
					tm_putChar('%');
					break;
				default:
					tm_putChar('?');
					break;
			}
		}
		else { tm_putChar(format_c); }
		format_c = hal_string_getChar(&format, format_index++);
	}

	tm_putChar(0); // close string
	return buff.index;
}
