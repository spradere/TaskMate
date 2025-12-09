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
 * @file snprintf.c
 * @brief implementation of libc formated print in string
 *
 */

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

#include "hal/hal_user_api.h"

#define SNPRINFT_BUFF_TEMP_SIZE 32


void baseConvert(char *buff_data, uint16_t *buff_index, size_t buff_size, uint32_t value, uint8_t base )
{
	static const char digits[] = "0123456789abcdef";

	char tmp[SNPRINFT_BUFF_TEMP_SIZE];
	uint8_t pos = 0;

	// reverse order convert
	while (value != 0 && pos < SNPRINFT_BUFF_TEMP_SIZE)
	{
		uint32_t d = (uint32_t)(value % base);
		value /= base;
		tmp[pos++] = digits[d];
	}

	// reverse order
	while ( (pos > 0) && (*buff_index < (buff_size-1)) )
	{
		buff_data[(*buff_index)++]= tmp[--pos];
	}
	buff_data[*buff_index]=0;

}

// !! use this macro only in tm_snprinf()
#define put_char(ch) do{ if(buff_index +1 < buff_size){buff[buff_index++]=(char)ch;}}while(0)

void snprintf(char *buff, size_t buff_size, const char *format, ...)
{
	uint16_t buff_index = 0;

	va_list args;
	va_start(args, format);

	while( *format )
	{
		if( *format == '%' && *(format + 1) )
		{
			format++;
			switch( *format )
			{

				case 'c':
				{
					int c = va_arg(args,int);
					put_char(c);
					break;
				}
				case 's':
				{
					char *s = va_arg(args, char *);
					while (*s)
					{
						//hal_usartWriteChar(*s);
						//hal_usartSendTXBuffer();
						put_char(*s);
						s++;
					}
					break;
				}

				case 'i':
				case 'x':
				case 'b':
				{
					int value = va_arg(args, int);
					uint8_t base;

					switch(*format)
					{
						case 'i': base = 10; break;
						case 'x': base = 16; break;
						case 'b': base = 2;  break;
						default : base = 10; break;
					}

				baseConvert(buff, &buff_index, buff_size, value, base);
				break;
				}

				case '%': put_char('%');
					break;
				default: put_char('?');
					break;
			}
		}
		else { put_char(*format); }
		format++;
	}

	va_end(args);

	put_char(0); // close string
	buff[buff_size-1] = 0; // worst case close at the end of buffer
}

