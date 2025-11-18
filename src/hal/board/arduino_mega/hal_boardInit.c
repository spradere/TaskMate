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
 * @file hal_boardinit.c
 * @brief board init arduino mega implematation
 *
 * @todo nothing
 */

#include "hal/hal_api.h"
#include "hal/board/arduino_mega/board_define.h"


void hal_boardInit()
{
	// Set pin output for in board led
	IN_BOARD_LED_DDR |= (1 << IN_BOARD_LED_PIN);

}


