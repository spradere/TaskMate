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
 * @file hal_gpio.c
 * @brief hal gpio implementation
 *
 */

#include <stdbool.h>
#include "hal/hal_user_api.h"
#include "sysCall/gpio.h"

void hal_gpioInitPin(gpio_pin_item_t *pin)
{
	if( pin->mode == GPIO_PIN_MODE_INPUT){pin->direction &= ~(1 << pin->number);}
	if( pin->mode == GPIO_PIN_MODE_OUTPUT_PP){pin->direction |= (1 << pin->number);}

	if( pin->pull == GPIO_PIN_PULL_UP){ pin->port |= (1 << pin->number);}
}

void hal_gpioWritePin(gpio_pin_item_t *pin, bool value)
{
	if(value == 0){pin->port &= ~(1 << pin->number);}
	else {pin->port |= (1 << pin->number);}
}

bool hal_gpioReadPin(gpio_pin_item_t *pin)
{
	return (pin->port & (1 << pin->number)) >> pin->number;
}
