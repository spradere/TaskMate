/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_usart_impl.h
 * @brief atmega2560 hal_usart_impl header declarations.
 *
 */

#ifndef HAL_USART_IMPL_H
#define HAL_USART_IMPL_H

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/auto_error_catalog.h"

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
