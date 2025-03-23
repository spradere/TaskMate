
/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file TaskMate_error.h
 * @brief System wide error definitions
 *
 * @todo Add event more error codes
 */

#ifndef TASKMATE_ERROR_H
#define TASKMATE_ERROR_H

typedef enum
{
	ERR_SUCCESS = 0, // No error
	ERR_INVALID_ARG = 1, // Non-critical: Task should handle it
	ERR_TIMEOUT = 2, // Non-critical: Task can retry or abort

	ERR_USART_TX_BUFFER_EMPTY = 100,
	ERR_USART_RX_BUFFER_EMPTY = 101,
	ERR_USART_TX_BUFFER_FULL = 102,
	ERR_USART_RX_BUFFER_FULL = 103,

	ERR_FATAL_STACK_OVERFLOW = 200, // Critical: OS must take action
	ERR_FATAL_MEMORY_CORRUPTION = 201, // Critical: OS must kill task
	ERR_FATAL_HW_FAILURE = 202 // Critical: OS must halt or reboot
} errorCode_t;

void setCriticalError(errorCode_t);

#endif
