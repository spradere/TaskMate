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
 */
#include <avr/io.h>
#include <stdbool.h>
#include "hal/hal_user_api.h"

void hal_boardInit()
{
	// Set pin output for in board led
	IN_BOARD_LED_DDR |= (1 << IN_BOARD_LED_PIN);

	// gpio pin definition
	gpio_signal_item_t sig;

	sig.pin.port = PORTB;
	sig.pin.direction = DDRB;
	sig.pin.number = PB7;
	sig.pin.mode = GPIO_PIN_MODE_OUTPUT_PP;
	sig.pin.pull = GPIO_PIN_PULL_NONE;
	sig.active_high = true;

	gpioSignalInit(GPIO_SIGNAL_INBOARD_LED, &sig);

}
