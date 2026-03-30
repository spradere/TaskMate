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
 * @file panic.h
 * @brief public panic header declarations.
 *
 */

#ifndef PANIC_H
#define PANIC_H

#include "hal/public/hal_context.h"
#include "hal/public/hal_usart.h"

#define panic(msg)                                 \
	do {                                           \
		hal_clearGlobalInterupt();                 \
		hal_usartSendTXBuffer();                   \
		hal_usartWriteString("\nSYSTEM PANIC : "); \
		hal_usartSendTXBuffer();                   \
		hal_usartWriteString(msg);                 \
		hal_usartSendTXBuffer();                   \
		hal_usartWriteString(", halt.");           \
		hal_usartSendTXBuffer();                   \
		while( 1 );                                \
	} while( 0 )

#endif
