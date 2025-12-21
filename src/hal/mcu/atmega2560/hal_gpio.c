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

#include "hal/mcu/atmega2560/hal_gpio.h"
#include <stdbool.h>
#include <avr/io.h>


static const gpio_mcu_port_t gpioMcuPorts[GPIO_PORT_COUNT] = {
	[GPIO_PORT_A] =
		(gpio_mcu_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRA), (volatile uint8_t *)_SFR_MEM_ADDR(PORTA),
						  (volatile uint8_t *)_SFR_MEM_ADDR(PINA)},
	[GPIO_PORT_B] =
		(gpio_mcu_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRB), (volatile uint8_t *)_SFR_MEM_ADDR(PORTB),
						  (volatile uint8_t *)_SFR_MEM_ADDR(PINB)} /*,
[GPIO_PORT_C] = { DDRC, PORTC, PINC },
[GPIO_PORT_D] = { DDRD, PORTD, PIND },
[GPIO_PORT_E] = { DDRE, PORTE, PINE },
[GPIO_PORT_F] = { DDRF, PORTF, PINF },
[GPIO_PORT_G] = { DDRG, PORTG, PING },
[GPIO_PORT_H] = { DDRH, PORTH, PINH },
[GPIO_PORT_I] = { DDRI, PORTI, PINI },
[GPIO_PORT_J] = { DDRJ, PORTJ, PINJ },
[GPIO_PORT_K] = { DDRK, PORTK, PINK },
[GPIO_PORT_L] = { DDRL, PORTL, PINL }*/

};

void hal_gpioInitPin(const gpio_pin_item_t *pin)
{

	if( pin->mode == GPIO_PIN_MODE_INPUT )
	{
		*(gpioMcuPorts[pin->port_index].ddr) &= (uint8_t)~(1u << pin->number);
	}
	if( pin->mode == GPIO_PIN_MODE_OUTPUT_PP )
	{
		*(gpioMcuPorts[pin->port_index].ddr) |= (uint8_t)(1u << pin->number);
	}

	if( pin->pull == GPIO_PIN_PULL_UP )
	{
		*(gpioMcuPorts[pin->port_index].port) |= (uint8_t)(1u << pin->number);
	}
}

void hal_gpioWritePin(const gpio_pin_item_t *pin, bool value)
{
	if( value ) { *(gpioMcuPorts[pin->port_index].port) |= (uint8_t)(1u << pin->number); }
	else { *(gpioMcuPorts[pin->port_index].port) &= (uint8_t)~(1u << pin->number); }
}

bool hal_gpioReadPin(const gpio_pin_item_t *pin)
{
	return (*(gpioMcuPorts[pin->port_index].port) & (1 << pin->number)) >> pin->number;
}
