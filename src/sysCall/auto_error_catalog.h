/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ! Auto generated code, do not edit !
// ! any changes will be lost         !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// clang-format off
#ifndef AUTO_ERROR_CATALOG_H
#define AUTO_ERROR_CATALOG_H

#include "tm_libc/tm_string.h"

typedef enum
{
	ERROR_LOW,
	ERROR_MID,
	ERROR_HIGH
} err_critical_t;

typedef enum
{
	ERR_NO_ERROR,
	ERR_HAL_USART_TX_BUFFER_EMPTY,
	ERR_HAL_USART_RX_BUFFER_EMPTY,
	ERR_HAL_USART_TX_BUFFER_FULL,
	ERR_HAL_USART_RX_BUFFER_FULL,
	ERR_MSG_OUT_OF_FREE_CHANNEL,
	ERROR_COUNT
} err_codes_t;

typedef struct
{
	const tm_string_t *name;
	const err_critical_t critical;
} err_item_t;

// clang-format on

#endif
