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

#include "interfaces/drivers.h"
#include "interfaces/macros.h"
#include "mcu_define.h" // get usart baud rate

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
static hal_driver_status_t usart_status;

static uint8_t hal_usartInit(void)
{
	uint16_t ubrr = (F_CPU / (16UL * USART_BAUD_RATE)) - 1;

	UBRR1H = (uint8_t)(ubrr >> 8);
	UBRR1L = (uint8_t)ubrr;

	TM_WRITEBIT(UCSR1B, RXEN1, TXEN1); // Enable Rx and Tx
	TM_WRITEBIT(UCSR1C, UCSZ11, UCSZ10); // 8-bit data, 1 stop bit, no parity

	hal_usartControl(TM_DRIVER_STATUS_SETBIT, TM_DRIVER_BIT_INIT);
	return 0;
}

static uint8_t hal_usartStart(void)
{
	if( (hal_usartControl(TM_DRIVER_STATUS_GETBIT, TM_DRIVER_BIT_INIT) == 0) ||
		(hal_usartControl(TM_DRIVER_STATUS_GETBIT, TM_DRIVER_BIT_DEAD) != 0) )
	{
		return TM_DRIVER_UNKNOW;
	}

	TM_SETBIT(UCSR1B, RXCIE1); // enable Rx interrupt

	hal_usartControl(TM_DRIVER_STATUS_SETBIT, TM_DRIVER_BIT_START);
	return 0;
}

static uint8_t hal_usartStop(void)
{
	// nothing to do ?
	hal_usartControl(TM_DRIVER_STATUS_CLEARBIT, TM_DRIVER_BIT_START);
	return 0;
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
		while( !TM_GETBIT(UCSR1A, UDRE1) ); // Wait for empty transmit buffer
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

uint8_t hal_usartControl(uint8_t cmd, uint8_t val)
{
	switch( cmd )
	{
		case TM_DRIVER_CTRL_INIT:
			return hal_usartInit();
		case TM_DRIVER_CTRL_START:
			return hal_usartStart();
		case TM_DRIVER_CTRL_STOP:
			return hal_usartStop();
		case TM_DRIVER_STATUS_RLSET:
			usart_status &= (hal_driver_status_t)~TM_DRIVER_RL_MASK;
			usart_status |= val;
			return 0;
		case TM_DRIVER_STATUS_RLGET:
			return usart_status & TM_DRIVER_RL_MASK;
		case TM_DRIVER_STATUS_SETBIT:
			TM_SETBIT(usart_status, val);
			return 0;
		case TM_DRIVER_STATUS_CLEARBIT:
			TM_CLEARBIT(usart_status, val);
			return 0;
		case TM_DRIVER_STATUS_GETBIT:
			return TM_GETBIT(usart_status, val);
		default:
			return TM_DRIVER_UNKNOW;
	}
}
