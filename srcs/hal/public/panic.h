/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file panic.h
 * @brief panic header declarations.
 *
 */

#ifndef HAL_PUBLIC_PANIC_H
#define HAL_PUBLIC_PANIC_H

#include "hal/public/int.h"
#include "hal/public/usart.h"

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

#endif // HAL_PUBLIC_PANIC_H
