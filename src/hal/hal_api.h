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
 * @file hal_api.h
 * @brief hal header api
 *
 * @todo nothing
 */

#ifndef HAL_API_H
#define HAL_API_H

#include <stdint.h>

void hal_archInit();
void hal_mcuInit();
void hal_boardInit();

#define HAL_IN_BOARD_LED_OFF 0
#define HAL_IN_BOARD_LED_ON 1
#define HAL_IN_BOARD_LED_TOGGLE 2

void hal_inBoardLed(uint8_t action);

void hal_timerSchedulerInit();
void hal_timerSchedulerStart();
void hal_timerSchedulerStop();
void hal_timerSchedulerLoad();

void hal_timerRTCInit();
void hal_timerRTCStart();
void hal_timerRTCStop();

void hal_threadContextInit(void (*func)(void), stack_word_t **stack_pointer, stack_word_t  *stack_top);

#endif
