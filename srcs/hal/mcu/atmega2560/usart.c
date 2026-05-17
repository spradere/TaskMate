/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file usart.c
 * @brief usart implementation.
 *
 */

#include "hal/mcu/atmega2560/usart.h"

#include <avr/interrupt.h>

#include "hal/mcu/atmega2560/mcu_define.h" // get usart baud rate

// Circular buffers
// always use a power of two for buffer size to avoid use of modulo
#define HAL_USART_BUFFER_SIZE 64

_Static_assert((HAL_USART_BUFFER_SIZE & (HAL_USART_BUFFER_SIZE - 1)) == 0,
			   "HAL_USART_BUFFER_SIZE must be a power of two");

_Static_assert((HAL_USART_BUFFER_SIZE <= 256), "HAL_USART_BUFFER_SIZE must be 256 max");

#define CB_MASK (HAL_USART_BUFFER_SIZE - 1)
#define CB_NEXT(index) (((index) + 1) & CB_MASK)
#define CB_FULL(head, tail) (CB_NEXT(head) == (tail))
#define CB_EMPTY(head, tail) ((head) == (tail))

static volatile uint8_t buffer_rx[HAL_USART_BUFFER_SIZE];
static volatile uint8_t buffer_tx[HAL_USART_BUFFER_SIZE];
static volatile uint8_t buffer_rx_head = 0, buffer_rx_tail = 0;
static volatile uint8_t buffer_tx_head = 0, buffer_tx_tail = 0;

void hal_usartInit(void)
{
	uint16_t ubrr = (F_CPU / (16UL * USART_BAUD_RATE)) - 1;

	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;

	UCSR1B = (1 << RXEN1) | (1 << TXEN1); // Enable Tx Rx
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); // 8-bit data, 1 stop bit, no parity
}

void hal_usartStart(void)
{
	UCSR1B |= (uint8_t)(1u << RXCIE1); // enable Rx interrupt
}

void hal_usartStop(void)
{
	// nothing to do ?
}

// USART1 Rx Interrupt Handler (Triggered when data is received)
ISR(USART1_RX_vect)
{
	uint8_t next_head = CB_NEXT(buffer_rx_head);
	uint8_t data = UDR1; // Read the received byte

	if( !CB_FULL(buffer_rx_head, buffer_rx_tail) )
	{
		buffer_rx[buffer_rx_head] = data;
		buffer_rx_head = next_head;
	}
}

// Read a character from Rx buffer (non-blocking)
err_codes_t hal_usartRead(uint8_t *data)
{
	if( CB_EMPTY(buffer_rx_head, buffer_rx_tail) ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }

	*data = buffer_rx[buffer_rx_tail];
	buffer_rx_tail = CB_NEXT(buffer_rx_tail);
	return ERR_NO_ERROR;
}

// Write a character to Tx buffer
err_codes_t hal_usartWriteChar(uint8_t data)
{
	uint8_t next_head = CB_NEXT(buffer_tx_head);
	if( CB_FULL(buffer_tx_head, buffer_tx_tail) ) { return ERR_HAL_USART_TX_BUFFER_FULL; }

	buffer_tx[buffer_tx_head] = data;
	buffer_tx_head = next_head;
	return ERR_NO_ERROR;
}

// send Tx buffer to usart
void hal_usartSendTXBuffer(void)
{
	while( !CB_EMPTY(buffer_tx_head, buffer_tx_tail) )
	{
		while( !(UCSR1A & (1 << UDRE1)) ); // Wait for empty transmit buffer
		UDR1 = buffer_tx[buffer_tx_tail]; // Put data into buffer, sends the data

		buffer_tx_tail = CB_NEXT(buffer_tx_tail);
	}
}

// test Rx buffer
err_codes_t hal_usartTestBufferRx(void)
{
	if( CB_EMPTY(buffer_rx_head, buffer_rx_tail) ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }
	if( CB_FULL(buffer_rx_head, buffer_rx_tail) ) { return ERR_HAL_USART_RX_BUFFER_FULL; }

	return ERR_NO_ERROR;
}

// test Tx buffer
err_codes_t hal_usartTestBufferTx(void)
{
	if( CB_EMPTY(buffer_tx_head, buffer_tx_tail) ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }
	if( CB_FULL(buffer_tx_head, buffer_tx_tail) ) { return ERR_HAL_USART_RX_BUFFER_FULL; }

	return ERR_NO_ERROR;
}

// write string to Tx buffer
err_codes_t hal_usartWriteString(const char *str)
{
	while( *str )
	{
		if( hal_usartWriteChar((uint8_t)*str++) == ERR_HAL_USART_TX_BUFFER_FULL )
		{
			return ERR_HAL_USART_TX_BUFFER_FULL;
		};
	}
	return ERR_NO_ERROR;
}
