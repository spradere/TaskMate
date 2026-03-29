/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file msg.h
 * @brief header of message display server
 *
 */

#ifndef MSG_H
#define MSG_H

#include <stdint.h>

#include "interfaces/auto_error_catalog.h"

// bits [2 1 0] is destination
#define MSG_TO_MASK 0x07
#define MSG_TO_NULL 0
#define MSG_TO_LCD 1
#define MSG_TO_USART 2

void msg(void);

err_codes_t msgRequestChannel(uint8_t *channel_id);
void msgWritreText(uint8_t channel_id, const char *msg, uint8_t dest);
void msgFreeChannel(uint8_t channel);

#endif
