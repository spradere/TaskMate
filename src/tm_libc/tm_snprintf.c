/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file tm_snprintf.c
 * @brief implementation of libc formatted print in string
 *
 */

#include "tm_libc/tm_snprintf.h"

#define SNPRINFT_BUFF_TEMP_SIZE 32

static void baseConvert(char *buff_data, uint8_t *buff_index, uint8_t buff_size, uint16_t value, uint8_t base)
{
	const char digits[] = "0123456789abcdef";

	char tmp[SNPRINFT_BUFF_TEMP_SIZE];
	uint8_t pos = 0;

	if( value == 0 )
	{
		buff_data[(*buff_index)++] = '0';
		return;
	}

	// reverse order convert
	while( (value != 0) && (pos < SNPRINFT_BUFF_TEMP_SIZE) )
	{
		uint16_t data = (value % base);
		value /= base;
		tmp[pos++] = digits[data];
	}

	// reverse order
	while( (pos > 0) && (*buff_index < (buff_size - 1)) ) { buff_data[(*buff_index)++] = tmp[--pos]; }
	buff_data[*buff_index] = 0;
}

int tm_snprintf(char *buff, uint8_t buff_size, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	tm_vsnprintf(buff, buff_size, format, args);
	va_end(args);
}

// !! use this macro only in TaskMate snprinf()
#define put_char(ch)                                                                                         \
	do {                                                                                                     \
		if( (uint8_t)(buff_index + 1) < buff_size ) { buff[buff_index++] = (char)(ch); }                     \
	} while( 0 )

int tm_vsnprintf(char *buff, uint8_t buff_size, const char *format, va_list args)
{
	uint8_t buff_index = 0;

	while( *format )
	{
		if( *format == '%' && *(format + 1) )
		{
			format++;
			switch( *format )
			{

				case 'c':
				{
					int c = va_arg(args, int);
					put_char(c);
					break;
				}
				case 's':
				{
					const char *s = va_arg(args, char *);
					while( *s )
					{
						put_char(*s);
						s++;
					}
					break;
				}

				case 'i':
				case 'x':
				case 'b':
				{
					uint16_t value = va_arg(args, uint16_t);
					uint8_t base;

					switch( *format )
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
					baseConvert(buff, &buff_index, buff_size, value, base);
					break;
				}

				case '%':
					put_char('%');
					break;
				default:
					put_char('?');
					break;
			}
		}
		else { put_char(*format); }
		format++;
	}

	put_char(0); // close string
	buff[buff_size - 1] = 0; // worst case close at the end of buffer
	return buff_index;
}
