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
 * @file tm_syslog.h
 * @brief header of system log
 *
 */

#ifndef TM_SYSLOG_H
#define TM_SYSLOG_H

// clang-format off

#include <avr/pgmspace.h>

#include "TaskMate.h" // get libc selection

#if TM_LIBC_CSTD
	#include <syslog.h>
	#define tm_syslog syslog
#endif

#if TM_LIBC_TASKMATE
	#include <stdarg.h>
	#include <stdint.h>
	void tm_syslog(const char *format, ...);
	void tm_vsyslog(const char *format, va_list args);
#endif

// clang-format on

#endif
