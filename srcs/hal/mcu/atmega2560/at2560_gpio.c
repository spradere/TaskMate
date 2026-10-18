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

#include <avr/io.h>
#include <stdbool.h>
#include <stddef.h>

#include "hal/arch/avr8/avr8_port.h"
#include "hal/mcu/atmega2560/at2560_ports_pins.h"
#include "interfaces/drv_gpio.h"
#include "interfaces/tm_macros.h"
#include "interfaces/tm_runLevel.h"

/* -----------------------------------------------
 * Private definitions
 * ---------------------------------------------*/

/**
 * @brief Maps a logical signal to a pin and active level.
 */
typedef struct
{
	hal_pin_t pin;
	bool active_high;
} hal_signal_t;

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static const avr8_port_t mcu_ports[PORT_COUNT] = {
	[PORT_A] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRA),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTA),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINA)},
	[PORT_B] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRB),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTB),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINB)},
	[PORT_C] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRC),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTC),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINC)},
	[PORT_D] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRD),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTD),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PIND)},
	[PORT_E] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRE),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTE),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINE)},
	[PORT_F] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRF),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTF),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINF)},
	[PORT_G] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRG),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTG),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PING)},
	[PORT_H] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRH),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTH),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINH)},
	[PORT_J] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRJ),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTJ),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINJ)},
	[PORT_K] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRK),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTK),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINK)},
	[PORT_L] = (avr8_port_t){(volatile uint8_t *)_SFR_MEM_ADDR(DDRL),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PORTL),
							 (volatile uint8_t *)_SFR_MEM_ADDR(PINL)},
};

static hal_signal_t signal_table[GPIO_SIGNAL_COUNT];
static hal_driver_status_t gpio_status;
static err_codes_t gpio_last_error = ERR_NO_ERROR;

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void gpioPinInit(const hal_pin_t *pin);
static void gpioPinWrite(hal_pin_t pin, bool value);
static bool gpioPinRead(hal_pin_t pin);

// [autoCode_tag] wire_gpio
#include "wire_gpio.inc"
// [/tag]

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

/* -----------------------------------------------
 * Driver life cycle
 * ---------------------------------------------*/

static hal_driver_state_t gpioSetError(err_codes_t error)
{
	gpio_last_error = error;
	return DRV_STATE_ERROR;
}

static hal_driver_state_t hal_gpioGetStatus(void)
{
	if( TM_GETBIT(gpio_status, DRV_BIT_DEAD) != 0U )
	{
		gpio_last_error = ERR_HAL_DRIVER_DEAD;
		return DRV_STATE_DEAD;
	}
	if( TM_GETBIT(gpio_status, DRV_BIT_ERROR) != 0U )
	{
		return gpioSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(gpio_status, DRV_BIT_INIT) == 0U )
	{
		if( TM_GETBIT(gpio_status, DRV_BIT_START) == 0U ) { return DRV_STATE_OFF; }
		return gpioSetError(ERR_HAL_DRIVER_INVALID_STATE);
	}
	if( TM_GETBIT(gpio_status, DRV_BIT_START) == 0U ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static bool gpioRequireRunning(void)
{
	hal_driver_state_t state = hal_gpioGetStatus();
	if( state == DRV_STATE_RUNNING ) { return true; }
	if( (state == DRV_STATE_OFF) || (state == DRV_STATE_INITIALIZED) )
	{
		gpioSetError(ERR_HAL_DRIVER_NOT_RUNNING);
	}
	return false;
}

static hal_driver_state_t hal_gpioInit(void)
{
	if( TM_GETBIT(gpio_status, DRV_BIT_DEAD) != 0U ) { return gpioSetError(ERR_HAL_DRIVER_DEAD); }
	for( uint8_t i = 0U; i < GPIO_SIGNAL_COUNT; i++ ) { targetWireSignal((gpio_signal_t)i); }
	TM_SETBIT(gpio_status, DRV_BIT_INIT);
	gpio_last_error = ERR_NO_ERROR;
	return DRV_STATE_INITIALIZED;
}

static hal_driver_state_t hal_gpioStart(void)
{
	if( TM_GETBIT(gpio_status, DRV_BIT_DEAD) != 0U ) { return gpioSetError(ERR_HAL_DRIVER_DEAD); }
	if( TM_GETBIT(gpio_status, DRV_BIT_INIT) == 0U )
	{
		return gpioSetError(ERR_HAL_DRIVER_NOT_INITIALIZED);
	}
	for( uint8_t i = 0U; i < GPIO_SIGNAL_COUNT; i++ ) { gpioPinInit(&signal_table[i].pin); }
	TM_SETBIT(gpio_status, DRV_BIT_START);
	gpio_last_error = ERR_NO_ERROR;
	return DRV_STATE_RUNNING;
}

static hal_driver_state_t hal_gpioStop(void)
{
	TM_CLEARBIT(gpio_status, DRV_BIT_START);
	return hal_gpioGetStatus();
}

/* -----------------------------------------------
 * GPIO operations
 * ---------------------------------------------*/

static void gpioPinInit(const hal_pin_t *pin)
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

void hal_gpioSignalWrite(gpio_signal_t signal, bool value)
{
	if( !gpioRequireRunning() ) { return; }
	gpioPinWrite(signal_table[signal].pin, value);
}

bool hal_gpioSignalRead(gpio_signal_t signal)
{
	if( !gpioRequireRunning() ) { return false; }
	return gpioPinRead(signal_table[signal].pin);
}

static void gpioPinWrite(hal_pin_t pin, bool value)
{
	if( !gpioRequireRunning() ) { return; }
	if( value ) { TM_SETBIT(*(mcu_ports[pin.port].port), pin.number); }
	else { TM_CLEARBIT(*(mcu_ports[pin.port].port), pin.number); }
}

static bool gpioPinRead(hal_pin_t pin)
{
	if( !gpioRequireRunning() ) { return false; }
	if( pin.mode == GPIO_PIN_MODE_INPUT )
	{
		return (*(mcu_ports[pin.port].pin) & (1U << pin.number)) >> pin.number;
	}
	return (*(mcu_ports[pin.port].port) & (1U << pin.number)) >> pin.number;
}

/* -----------------------------------------------
 * Driver control
 * ---------------------------------------------*/

hal_driver_state_t hal_gpioControl(hal_driver_control_t command, hal_driver_control_data_t *data)
{
	switch( command )
	{
		case DRV_CTRL_INIT:
			return hal_gpioInit();
		case DRV_CTRL_START:
			return hal_gpioStart();
		case DRV_CTRL_STOP:
			return hal_gpioStop();
		case DRV_CTRL_RLSET:
			if( data == NULL ) { return gpioSetError(ERR_NULL_POINTER); }
			if( data->run_level >= RL_LEVEL_COUNT )
			{
				return gpioSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			gpio_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			gpio_status |= data->run_level;
			return hal_gpioGetStatus();
		case DRV_CTRL_RLGET:
			if( data == NULL ) { return gpioSetError(ERR_NULL_POINTER); }
			data->run_level = RL_GET_RUN_LEVEL(gpio_status);
			return hal_gpioGetStatus();
		case DRV_CTRL_SETBIT:
			if( data == NULL ) { return gpioSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return gpioSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_SETBIT(gpio_status, data->status_bit);
			return hal_gpioGetStatus();
		case DRV_CTRL_CLEARBIT:
			if( data == NULL ) { return gpioSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return gpioSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			TM_CLEARBIT(gpio_status, data->status_bit);
			return hal_gpioGetStatus();
		case DRV_CTRL_GETBIT:
			if( data == NULL ) { return gpioSetError(ERR_NULL_POINTER); }
			if( (data->status_bit < DRV_BIT_INIT) || (data->status_bit > DRV_BIT_DEAD) )
			{
				return gpioSetError(ERR_HAL_DRIVER_INVALID_VALUE);
			}
			data->bit_value = TM_GETBIT(gpio_status, data->status_bit) != 0U;
			return hal_gpioGetStatus();
		case DRV_CTRL_GETSTATUS:
			return hal_gpioGetStatus();
		case DRV_CTRL_GETLASTERROR:
			if( data == NULL ) { return gpioSetError(ERR_NULL_POINTER); }
			data->error = gpio_last_error;
			return hal_gpioGetStatus();
		default:
			return gpioSetError(ERR_HAL_DRIVER_INVALID_CONTROL);
	}
}
