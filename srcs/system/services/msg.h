/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file msg.h
 * @brief msg header declarations.
 *
 */

#ifndef SERVICES_MSG_H
#define SERVICES_MSG_H

#include <stdint.h>

#include "interfaces/error_catalog.h"
#include "interfaces/tm_string_storage.h"

// bits [2 1 0] is destination
#define MSG_TO_MASK 0x07
#define MSG_TO_NULL 0
#define MSG_TO_LCD 1
#define MSG_TO_USART 2

void msg(void);

err_codes_t msgRequestChannel(uint8_t *channel_id);
void msgWriteText(uint8_t channel_id, tm_string_t msg, uint8_t dest);
void msgFreeChannel(uint8_t channel);

#endif // SERVICES_MSG_H
