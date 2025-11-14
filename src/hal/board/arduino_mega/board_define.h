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
 * @file board_define.h
 * @brief arduino mega definitions
 *
 * @todo nothing
 */

#ifndef BOARD_DEFINE_H
#define BOARD_DEFINE_H

#include <avr/io.h>

// arduino in board LED pin 13
#define IN_BOARD_LED_DDR DDRB /**< Arduino in board led, direction port*/
#define IN_BOARD_LED_PORT PORTB /**< Arduino in board led, data port*/
#define IN_BOARD_LED_PIN PB7 /**< Arduino in board led, pin*/

#endif
