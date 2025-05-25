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
 * @file printf.c
 * @brief implementation of libc print formated
 *
 * @todo nothing
 */

#include <avr/io.h>
#include <stdarg.h>

// public function
void snprintf(char *buf, uint8_t n, const char *format, ...)
{
	va_list args;
    va_start(args, format);

    while (*format)
    {
        if (*format == '%' && *(format + 1))
        {
            format++;
            switch (*format)
            {
                case 's': /*uart_puts(va_arg(args, char*));*/ break;
                case 'c': /*uart_putc((char)va_arg(args, int));*/ break;
                case 'd': /*print_decimal(va_arg(args, int));*/ break;
                case 'x': /*print_hex(va_arg(args, unsigned int));*/ break;
                case '%': /*uart_putc('%');*/ break;
                default: /*uart_putc('?');*/
            }
        }
        else
        {
            /*uart_putc(*format);*/
        }
        format++;
    }

    va_end(args);
}
