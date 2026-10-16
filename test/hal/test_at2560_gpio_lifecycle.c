/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file test_at2560_gpio_lifecycle.c
 * @brief Exercise GPIO wiring, startup, and stopped I/O on host registers.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

/* -----------------------------------------------
 * Host register model
 * ---------------------------------------------*/

#define _SFR_MEM_ADDR(reg) (&(reg))
#define PA0 0U
#define PA1 1U
#define PB7 7U

#define GPIO_TEST_PORT(letter)       \
	static volatile uint8_t DDR##letter;  \
	static volatile uint8_t PORT##letter; \
	static volatile uint8_t PIN##letter

GPIO_TEST_PORT(A);
GPIO_TEST_PORT(B);
GPIO_TEST_PORT(C);
GPIO_TEST_PORT(D);
GPIO_TEST_PORT(E);
GPIO_TEST_PORT(F);
GPIO_TEST_PORT(G);
GPIO_TEST_PORT(H);
GPIO_TEST_PORT(J);
GPIO_TEST_PORT(K);
GPIO_TEST_PORT(L);

#undef GPIO_TEST_PORT

#include "hal/mcu/atmega2560/at2560_gpio.c"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

int main(void)
{
	hal_driver_control_data_t data;

	assert(hal_gpioControl(DRV_CTRL_GETSTATUS, NULL) == DRV_STATE_OFF);
	assert(hal_gpioControl(DRV_CTRL_START, NULL) == DRV_STATE_ERROR);
	assert(hal_gpioControl(DRV_CTRL_GETLASTERROR, &data) == DRV_STATE_OFF);
	assert(data.error == ERR_HAL_DRIVER_NOT_INITIALIZED);

	assert(hal_gpioControl(DRV_CTRL_INIT, NULL) == DRV_STATE_INITIALIZED);
	data.run_level = RL_RUN_NONE;
	assert(hal_gpioControl(DRV_CTRL_RLSET, &data) == DRV_STATE_INITIALIZED);
	assert(hal_gpioControl(DRV_CTRL_RLGET, &data) == DRV_STATE_INITIALIZED);
	assert(data.run_level == RL_RUN_NONE);
	assert((DDRA == 0U) && (DDRB == 0U));
	assert(signal_table[GPIO_SIGNAL_INBOARD_LED].pin.port == PORT_B);
	assert(signal_table[GPIO_SIGNAL_TASK1_LED].pin.port == PORT_A);
	assert(signal_table[GPIO_SIGNAL_TASK2_LED].pin.number == PA1);

	hal_gpioSignalWrite(GPIO_SIGNAL_TASK1_LED, true);
	assert(PORTA == 0U);
	assert(hal_gpioControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	assert((DDRA & (1U << PA0)) != 0U);
	assert((DDRB & (1U << PB7)) != 0U);

	hal_gpioSignalWrite(GPIO_SIGNAL_TASK1_LED, true);
	assert(hal_gpioSignalRead(GPIO_SIGNAL_TASK1_LED));
	assert((PORTA & (1U << PA0)) != 0U);
	assert(hal_gpioControl(DRV_CTRL_STOP, NULL) == DRV_STATE_INITIALIZED);
	assert((DDRA & (1U << PA0)) != 0U);
	assert((PORTA & (1U << PA0)) != 0U);

	hal_gpioSignalWrite(GPIO_SIGNAL_TASK1_LED, false);
	gpioPinWrite(signal_table[GPIO_SIGNAL_TASK1_LED].pin, false);
	assert((PORTA & (1U << PA0)) != 0U);
	assert(!hal_gpioSignalRead(GPIO_SIGNAL_TASK1_LED));
	assert(!gpioPinRead(signal_table[GPIO_SIGNAL_TASK1_LED].pin));
	assert(hal_gpioControl(DRV_CTRL_GETLASTERROR, &data) == DRV_STATE_INITIALIZED);
	assert(data.error == ERR_HAL_DRIVER_NOT_RUNNING);

	assert(hal_gpioControl(DRV_CTRL_START, NULL) == DRV_STATE_RUNNING);
	assert(hal_gpioSignalRead(GPIO_SIGNAL_TASK1_LED));
	hal_gpioSignalWrite(GPIO_SIGNAL_TASK1_LED, false);
	assert((PORTA & (1U << PA0)) == 0U);
	return 0;
}
