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
 * @file hal_usart.h
 * @brief header hal usart implemetation
 *
 */

#ifndef HAL_USART_H
#define HAL_USART_H

#include <stdint.h>
#include <stdbool.h>
#include "sysCall/error.h"

void hal_usartInit(void);
void hal_usartStart(void);
void hal_usartStop(void);
error_codes_t hal_usartRead(uint8_t *data);
error_codes_t hal_usartWriteChar(uint8_t data);
void hal_usartSendTXBuffer(void);
error_codes_t hal_usartTestBufferRx(void);
error_codes_t hal_usartTestBufferTx(void);
error_codes_t hal_usartWriteString(const char *str);

#endif
