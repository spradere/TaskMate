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
 * @file tm_syslog.c
 * @brief implementation of system log
 *
 */

#include "tm_libc/tm_syslog.h"

#include "hal/auto_hal_user.h"
#include "tm_libc/tm_snprintf.h"

static char tm_log[128];

// TODO add level INFO / WARN / ERROR

void tm_syslog(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	tm_vsyslog(format, args);
	va_end(args);
}

void tm_vsyslog(const char *format, va_list args)
{
	tm_vsnprintf(tm_log, sizeof(tm_log), format, args);

	hal_usartWriteString(tm_log);
	hal_usartSendTXBuffer();
}
