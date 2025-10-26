/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file usart1.h
 * @brief header for usart1 driver
 *
 * @todo Nothing
 */

#ifndef USART1_H
#define USART1_H

#include "sysCore/error.h"

void usart1Init(void);
void usart1Start(void);
void usart1Stop(void);

errorCode_t usart1Read(uint8_t *data);
errorCode_t usart1WriteChar(uint8_t data);
void usart1SendTXBuffer(void);

errorCode_t usart1TestBufferRx(void);
errorCode_t usart1WriteString(const char *str);

#endif
