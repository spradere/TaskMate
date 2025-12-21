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
 * @file hal_usart.c
 * @brief hal usart implemetation
 *
 */

#include <avr/interrupt.h>
#include "hal/mcu/atmega2560/hal_usart.h"
#include "hal/mcu/atmega2560/mcu_define.h"

// Circular buffers
#define HAL_USART_BUFFER_SIZE 128

static volatile uint8_t buffer_rx[HAL_USART_BUFFER_SIZE];
static volatile uint8_t buffer_tx[HAL_USART_BUFFER_SIZE];
static volatile uint8_t buffer_rx_head = 0, buffer_rx_tail = 0;
static volatile uint8_t buffer_tx_head = 0, buffer_tx_tail = 0;

void hal_usartInit(void)
{
	uint16_t ubrr = (F_CPU / (16UL * HAL_USART_BAUD_RATE)) - 1;

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
	uint8_t next_head = (uint8_t)((buffer_rx_head + 1) % HAL_USART_BUFFER_SIZE);
	uint8_t data = UDR1; // Read the received byte

	if( next_head != buffer_rx_tail ) // Check for buffer overflow
	{
		buffer_rx[buffer_rx_head] = data;
		buffer_rx_head = next_head; // Move head pointer forward
	}
}

// Read a character from Rx buffer (non-blocking)
error_codes_t hal_usartRead(uint8_t *data)
{
	if( buffer_rx_tail == buffer_rx_head ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }

	*data = buffer_rx[buffer_rx_tail]; // Read from buffer
	buffer_rx_tail = (uint8_t)((buffer_rx_tail + 1) % HAL_USART_BUFFER_SIZE); // Move tail forward
	return ERR_NO_ERROR;
}

// Write a character to Tx buffer
error_codes_t hal_usartWriteChar(uint8_t data)
{
	uint8_t next_head = (uint8_t)((buffer_tx_head + 1) % HAL_USART_BUFFER_SIZE);
	if( next_head == buffer_tx_tail ) { return ERR_HAL_USART_TX_BUFFER_FULL; }

	buffer_tx[buffer_tx_head] = data;
	buffer_tx_head = next_head;
	return ERR_NO_ERROR;
}

// send Tx buffer to usart
void hal_usartSendTXBuffer(void)
{
	while( buffer_tx_tail != buffer_tx_head ) // test if tx buffer empty
	{
		while( !(UCSR1A & (1 << UDRE1)) ); // Wait for empty transmit buffer
		UDR1 = buffer_tx[buffer_tx_tail]; // Put data into buffer, sends the data

		buffer_tx_tail = (uint8_t)((buffer_tx_tail + 1) % HAL_USART_BUFFER_SIZE); // Move tail forward
	}
}

// test if Rx buffer is empty
error_codes_t hal_usartTestBufferRx(void)
{
	if( buffer_rx_tail == buffer_rx_head ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }
	return ERR_NO_ERROR;
}

// write string to Tx buffer
error_codes_t hal_usartWriteString(const char *str)
{
	while( *str )
	{
		if( hal_usartWriteChar((uint8_t)*str++) == ERR_HAL_USART_TX_BUFFER_FULL ) { break; };
	}
	return ERR_NO_ERROR;
}
