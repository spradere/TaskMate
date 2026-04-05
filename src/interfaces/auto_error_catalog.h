/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
 */

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// ! Auto generated code, do not edit !
// ! any changes will be lost         !
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

// clang-format off
#ifndef AUTO_ERROR_CATALOG_H
#define AUTO_ERROR_CATALOG_H

typedef enum
{
	ERROR_LOW,
	ERROR_MID,
	ERROR_HIGH
} err_critical_t;

typedef enum
{
	ERR_HAL_USART_TX_BUFFER_EMPTY,
	ERR_HAL_USART_RX_BUFFER_EMPTY,
	ERR_HAL_USART_TX_BUFFER_FULL,
	ERR_HAL_USART_RX_BUFFER_FULL,
	ERR_MSG_OUT_OF_FREE_CHANNEL,
	ERR_NO_ERROR,
	ERR_NULL_POINTER,
	ERR_UNKNOW,
	ERR_RUNTIME,
	ERROR_COUNT
} err_codes_t;

// clang-format on

#endif
