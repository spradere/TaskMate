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
 * @file hal_inBoardLed.h
 * @brief hal in board led arduino mega implematation
 *
 * @todo nothing
 */

#include "sysCore/autoInclude.h"
#include "hal/board/arduino_mega/board_define.h"

void hal_inBoardLed(uint8_t action)
{

switch(action)
	{
		case HAL_IN_BOARD_LED_OFF: IN_BOARD_LED_PORT &= ~(1 << IN_BOARD_LED_PIN); break;
		case HAL_IN_BOARD_LED_ON: IN_BOARD_LED_PORT |= (1 << IN_BOARD_LED_PIN); break;
		case HAL_IN_BOARD_LED_TOGGLE: IN_BOARD_LED_PORT ^= (1 << IN_BOARD_LED_PIN); break;
		default: break;
	}
}
