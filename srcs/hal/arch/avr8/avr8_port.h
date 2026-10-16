/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file avr8_port.h
 * @brief AVR8 GPIO port register addresses.
 */

#ifndef AVR8_AVR8_PORT_H
#define AVR8_AVR8_PORT_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @brief Groups the data direction, output, and input registers of one AVR8 port.
 */
typedef struct
{
	volatile uint8_t *ddr;
	volatile uint8_t *port;
	volatile uint8_t *pin;
} avr8_port_t;

#endif // AVR8_AVR8_PORT_H
