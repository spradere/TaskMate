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

#include "usart.h"

#include <avr/interrupt.h>

#include "interfaces/macros.h"
#include "interfaces/modules_define.h"
#include "interfaces/runLevel_define.h"
#include "mcu_define.h" // Get the USART baud rate
#include "tmlibc.h"

// Circular buffers
// Always use a power of two for the buffer size to avoid modulo operations
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
static hal_driver_status_t usart_status;

static err_codes_t usartWriteChar(uint8_t data);

static uint8_t hal_usartGetStatus(void)
{
	if( TM_GETBIT(usart_status, DRV_BIT_ERROR) != 0 ) { return DRV_STATE_ERROR; }
	if( TM_GETBIT(usart_status, DRV_BIT_INIT) == 0 )
	{
		if( TM_GETBIT(usart_status, DRV_BIT_START) == 0 ) { return DRV_STATE_OFF; }
		return DRV_STATE_ERROR;
	}
	if( TM_GETBIT(usart_status, DRV_BIT_START) == 0 ) { return DRV_STATE_INITIALIZED; }
	return DRV_STATE_RUNNING;
}

static uint8_t hal_usartInit(void)
{
	uint16_t ubrr = (F_CPU / (16UL * USART_BAUD_RATE)) - 1;

	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;

	TM_WRITEBIT(UCSR1B, RXEN1, TXEN1); // Enable Rx and Tx
	TM_WRITEBIT(UCSR1C, UCSZ11, UCSZ10); // 8-bit data, 1 stop bit, no parity

	hal_usartControl(DRV_CTRL_SETBIT, DRV_BIT_INIT);
	return 0;
}

static uint8_t hal_usartStart(void)
{
	if( (hal_usartControl(DRV_CTRL_GETBIT, DRV_BIT_INIT) == 0) ||
		(hal_usartControl(DRV_CTRL_GETBIT, DRV_BIT_DEAD) != 0) )
	{
		return DRV_UNKNOW;
	}

	TM_SETBIT(UCSR1B, RXCIE1); // enable Rx interrupt

	hal_usartControl(DRV_CTRL_SETBIT, DRV_BIT_START);
	return 0;
}

static uint8_t hal_usartStop(void)
{
	// Nothing to do?
	hal_usartControl(DRV_CTRL_CLEARBIT, DRV_BIT_START);
	return 0;
}

// USART1 RX interrupt handler (triggered when data is received)
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

// Read a character from the RX buffer (non-blocking)
err_codes_t hal_usartRead(uint8_t *data)
{
	if( hal_usartGetStatus() != DRV_STATE_RUNNING ) { return ERR_RUNTIME; }
	if( CB_EMPTY(buffer_rx_head, buffer_rx_tail) ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }

	*data = buffer_rx[buffer_rx_tail];
	buffer_rx_tail = CB_NEXT(buffer_rx_tail);
	return ERR_NO_ERROR;
}

// Write a character to the TX buffer
static err_codes_t usartWriteChar(uint8_t data)
{
	uint8_t next_head = CB_NEXT(buffer_tx_head);
	if( CB_FULL(buffer_tx_head, buffer_tx_tail) ) { return ERR_HAL_USART_TX_BUFFER_FULL; }

	buffer_tx[buffer_tx_head] = data;
	buffer_tx_head = next_head;
	return ERR_NO_ERROR;
}

err_codes_t hal_usartWriteChar(uint8_t data)
{
	if( hal_usartGetStatus() != DRV_STATE_RUNNING ) { return ERR_RUNTIME; }
	return usartWriteChar(data);
}

// Send the TX buffer to the USART
err_codes_t hal_usartSendTXBuffer(void)
{
	if( hal_usartGetStatus() != DRV_STATE_RUNNING ) { return ERR_RUNTIME; }
	while( !CB_EMPTY(buffer_tx_head, buffer_tx_tail) )
	{
		while( !TM_GETBIT(UCSR1A, UDRE1) ); // Wait for empty transmit buffer
		UDR1 = buffer_tx[buffer_tx_tail]; // Put data into buffer, sends the data

		buffer_tx_tail = CB_NEXT(buffer_tx_tail);
	}
	return ERR_NO_ERROR;
}

// Test the RX buffer
err_codes_t hal_usartTestBufferRx(void)
{
	if( hal_usartGetStatus() != DRV_STATE_RUNNING ) { return ERR_RUNTIME; }
	if( CB_EMPTY(buffer_rx_head, buffer_rx_tail) ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }
	if( CB_FULL(buffer_rx_head, buffer_rx_tail) ) { return ERR_HAL_USART_RX_BUFFER_FULL; }

	return ERR_NO_ERROR;
}

// Test the TX buffer
err_codes_t hal_usartTestBufferTx(void)
{
	if( hal_usartGetStatus() != DRV_STATE_RUNNING ) { return ERR_RUNTIME; }
	if( CB_EMPTY(buffer_tx_head, buffer_tx_tail) ) { return ERR_HAL_USART_RX_BUFFER_EMPTY; }
	if( CB_FULL(buffer_tx_head, buffer_tx_tail) ) { return ERR_HAL_USART_RX_BUFFER_FULL; }

	return ERR_NO_ERROR;
}

// Write a string to the TX buffer
err_codes_t hal_usartWriteString(tm_string_t str)
{
	uint8_t index = 0;

	if( hal_usartGetStatus() != DRV_STATE_RUNNING ) { return ERR_RUNTIME; }
	if( str.text == 0 ) { return ERR_RUNTIME; }

	while( index < TM_STRING_SIZE_MAX )
	{
		char str_char = hal_string_getChar(&str, index);
		if( str_char == 0 ) { break; }
		if( usartWriteChar((uint8_t)str_char) == ERR_HAL_USART_TX_BUFFER_FULL )
		{
			return ERR_HAL_USART_TX_BUFFER_FULL;
		};
		index++;
	}
	return ERR_NO_ERROR;
}

uint8_t hal_usartControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case DRV_CTRL_INIT:
			return hal_usartInit();
		case DRV_CTRL_START:
			return hal_usartStart();
		case DRV_CTRL_STOP:
			return hal_usartStop();
		case DRV_CTRL_RLSET:
			usart_status &= (hal_driver_status_t)~RL_LEVEL_MASK;
			usart_status |= val;
			return 0;
		case DRV_CTRL_RLGET:
			return usart_status & RL_LEVEL_MASK;
		case DRV_CTRL_SETBIT:
			TM_SETBIT(usart_status, val);
			return 0;
		case DRV_CTRL_CLEARBIT:
			TM_CLEARBIT(usart_status, val);
			return 0;
		case DRV_CTRL_GETBIT:
			return TM_GETBIT(usart_status, val);
		case DRV_CTRL_GETSTATUS:
			return hal_usartGetStatus();
		default:
			return DRV_UNKNOW;
	}
}
