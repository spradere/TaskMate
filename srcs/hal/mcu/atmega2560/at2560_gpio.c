/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file at2560_gpio.c
 * @brief gpio implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "at2560_gpio.h"

#include <avr/io.h>
#include <stdbool.h>

#include "interfaces/gpio_signals.h"
#include "interfaces/tm_macros.h"

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static const hal_port_t mcu_ports[PORT_COUNT] = {
	[PORT_A] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRA),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTA),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINA)},
	[PORT_B] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRB),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTB),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINB)},
	[PORT_C] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRC),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTC),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINC)},
	[PORT_D] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRD),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTD),
							(volatile uint8_t *)_SFR_MEM_ADDR(PIND)},
	[PORT_E] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRE),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTE),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINE)},
	[PORT_F] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRF),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTF),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINF)},
	[PORT_G] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRG),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTG),
							(volatile uint8_t *)_SFR_MEM_ADDR(PING)},
	[PORT_H] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRH),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTH),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINH)},
	[PORT_J] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRJ),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTJ),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINJ)},
	[PORT_K] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRK),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTK),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINK)},
	[PORT_L] = (hal_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRL),
							(volatile uint8_t *)_SFR_MEM_ADDR(PORTL),
							(volatile uint8_t *)_SFR_MEM_ADDR(PINL)},
};

static hal_signal_t signal_table[GPIO_SIGNAL_COUNT];

// [autoCode_tag] wire_gpio
#include "wire_gpio.inc"
// [/tag]

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void hal_gpioPinInit(const hal_pin_t *pin)
{
	if( pin->mode == GPIO_PIN_MODE_INPUT )
	{
		TM_CLEARBIT(*(mcu_ports[pin->port].ddr), pin->number);
	}
	if( pin->mode == GPIO_PIN_MODE_OUTPUT_PP )
	{
		TM_SETBIT(*(mcu_ports[pin->port].ddr), pin->number);
	}

	if( pin->pull == GPIO_PIN_PULL_UP ) { TM_SETBIT(*(mcu_ports[pin->port].port), pin->number); }
}

void hal_gpioSignalInit(void)
{
	for( uint8_t i = 0U; i < GPIO_SIGNAL_COUNT; i++ )
	{
		targetWireSignal((gpio_signal_t)i);
		hal_gpioPinInit(&signal_table[i].pin);
	}
}

void hal_gpioSignalWrite(gpio_signal_t signal, bool value)
{
	hal_gpioPinWrite(signal_table[signal].pin, value);
}

bool hal_gpioSignalRead(gpio_signal_t signal) { return hal_gpioPinRead(signal_table[signal].pin); }

void hal_gpioPinWrite(const hal_pin_t pin, bool value)
{
	if( value ) { TM_SETBIT(*(mcu_ports[pin.port].port), pin.number); }
	else { TM_CLEARBIT(*(mcu_ports[pin.port].port), pin.number); }
}

bool hal_gpioPinRead(const hal_pin_t pin)
{
	if( pin.mode == GPIO_PIN_MODE_INPUT )
	{
		return (*(mcu_ports[pin.port].pin) & (1U << pin.number)) >> pin.number;
	}
	return (*(mcu_ports[pin.port].port) & (1U << pin.number)) >> pin.number;
}
