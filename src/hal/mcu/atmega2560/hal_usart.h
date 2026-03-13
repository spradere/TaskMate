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

/**
 * @file hal_usart.h
 * @brief header hal usart implementation
 *
 */

// @hal_user

#ifndef HAL_USART_H
#define HAL_USART_H

#include <stdbool.h>
#include <stdint.h>

#include "sysCall/error.h"

void hal_usartInit(void);
void hal_usartStart(void);
void hal_usartStop(void);
err_codes_t hal_usartRead(uint8_t *data);
err_codes_t hal_usartWriteChar(uint8_t data);
void hal_usartSendTXBuffer(void);
err_codes_t hal_usartTestBufferRx(void);
err_codes_t hal_usartTestBufferTx(void);
err_codes_t hal_usartWriteString(const char *str);

#endif
