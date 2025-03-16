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
 * @file usart1.h
 * @brief header for usart1 driver
 * 
 * 
 */
 
 
#ifndef USART1_H
#define USART1_H

#include <stdint.h>
#include "sysCore/TaskMate_error.h"

void usart1SetStatus(uint8_t status);
uint8_t usart1GetStatus(void);
uint8_t *usart1GetName(void);
void usart1Init(void);
void usart1Start(void);
void usart1Stop(void);

errorCode_t usart1Read(uint8_t *data);
errorCode_t usart1Write(uint8_t data);
void usart1Flush(void);

#endif
