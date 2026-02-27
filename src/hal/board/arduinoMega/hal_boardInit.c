/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file hal_boardInit.c
 * @brief board init arduino mega implementation
 *
 */

#include "hal/board/arduinoMega/hal_boardInit.h"

#include <avr/io.h>
#include <stdbool.h>

#include "hal/auto_hal_user.h"

void hal_boardInit(void)
{
	// gpio pin definition
	gpio_signal_item_t sig;

	sig.pin.port_index = GPIO_PORT_B;
	sig.pin.number = PB7;
	sig.pin.mode = GPIO_PIN_MODE_OUTPUT_PP;
	sig.pin.pull = GPIO_PIN_PULL_NONE;
	sig.active_high = true;
	gpio_signalInit(GPIO_SIGNAL_INBOARD_LED, &sig);

	sig.pin.port_index = GPIO_PORT_A;
	sig.pin.number = PA0;
	gpio_signalInit(GPIO_SIGNAL_TASK1_LED, &sig);

	sig.pin.port_index = GPIO_PORT_A;
	sig.pin.number = PA1;
	gpio_signalInit(GPIO_SIGNAL_TASK2_LED, &sig);
}
