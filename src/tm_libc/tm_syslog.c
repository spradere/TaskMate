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
 * @file tm_syslog.c
 * @brief implementation of system log
 *
 */

#include "tm_libc/tm_syslog.h"

#include "tm_libc/tm_stdio.h"

void tm_syslog(PGM_P format, ...)
{
	va_list args;
	va_start(args, format);
	tm_vsyslog(format, args);
	va_end(args);
}

void tm_vsyslog(PGM_P format, va_list args)
{
	tm_vprintf(format, args);
}
