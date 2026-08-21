/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file usart.h
 * @brief usart header declarations.
 *
 */

#ifndef ATMEGA2560_USART_H
#define ATMEGA2560_USART_H

#include <stdbool.h>
#include <stdint.h>

#include "interfaces/error_catalog.h"

uint8_t hal_usartInit(void);
uint8_t hal_usartStart(void);
uint8_t hal_usartStop(void);
err_codes_t hal_usartRead(uint8_t *data);
err_codes_t hal_usartWriteChar(uint8_t data);
void hal_usartSendTXBuffer(void);
err_codes_t hal_usartTestBufferRx(void);
err_codes_t hal_usartTestBufferTx(void);
err_codes_t hal_usartWriteString(const char *str);

#endif // ATMEGA2560_USART_H
