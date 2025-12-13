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
 * @file mcu_define.h
 * @brief atmega2560 definitions
 *
 */

#ifndef MCU_DEFINE_H
#define MCU_DEFINE_H

#define I2C_FREQ 100000UL // Standard mode 100 kHz
#define I2C_TWBR_VALUE ((F_CPU / I2C_FREQ - 16) / 2)

#define HAL_USART_BAUD_RATE 9600

// gpio atmega2560 ports
typedef enum
{
	GPIO_PORT_A,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E,
	GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_H,
	GPIO_PORT_I,
	GPIO_PORT_J,
	GPIO_PORT_K,
	GPIO_PORT_L,

	GPIO_PORT_COUNT
} gpio_port_index_t;

#endif
