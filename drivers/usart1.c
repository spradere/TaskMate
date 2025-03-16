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
 * @file usart1.c
 * @brief implementation of usart1 driver
 * 
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "sysCore/TaskMate_public.h"
#include "drivers/usart1.h"
#include "tasks/lcd.h"

#define BAUD_RATE 9600


uint8_t usart1_name[]="usart 1";
uint8_t usart1_status=0;

// Circular buffers
#define BUFFER_TX_SIZE_SIZE 256
#define BUFFER_TX_SIZE 256

static volatile uint8_t buffer_rx[BUFFER_TX_SIZE];
static volatile uint8_t buffer_tx[BUFFER_TX_SIZE];
static volatile uint8_t buffer_rx_head = 0, buffer_rx_tail = 0;
static volatile uint8_t buffer_tx_head = 0, buffer_tx_tail = 0;



void usart1SetStatus(uint8_t status)
{
	usart1_status=status;
}

uint8_t usart1GetStatus(void)
{
	return usart1_status;
}

uint8_t *usart1GetName(void)
{
	return &usart1_name[0];
}


void usart1Init(void)
{	
    uint16_t ubrr = (F_CPU / (16UL * BAUD_RATE)) - 1;
    
    UBRR1H = (uint8_t)(ubrr >> 8);
    UBRR1L = (uint8_t)ubrr;
    
    UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); // Enable Tx, Rx, and Rx interrupt
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10); // 8-bit data, 1 stop bit, no parity

}

void usart1Start(void)
{
	// start by enable INT
	
}

void usart1Stop(void)
{
	// nothing to do, will stop all system.
}

// USART1 Rx Interrupt Handler (Triggered when data is received)
ISR(USART1_RX_vect) 
{
    uint8_t next_head = (buffer_rx_head + 1) % BUFFER_TX_SIZE_SIZE;
    uint8_t data = UDR1; // Read the received byte
	
    if (next_head != buffer_rx_tail) // Check for buffer overflow
    {  
        buffer_rx[buffer_rx_head] = data;
        buffer_rx_head = next_head;  // Move head pointer forward
    }
}

// Read a character from Rx buffer (non-blocking)
int8_t usart1Read(uint8_t *data) 
{
    if (buffer_rx_tail==buffer_rx_head) {return -1;}  // Buffer empty

    *data = buffer_rx[buffer_rx_tail]; // Read from buffer
    buffer_rx_tail = (buffer_rx_tail + 1) % BUFFER_TX_SIZE_SIZE; // Move tail forward
    return 0; // Success
}

// Write a character to Tx buffer
void usart1Write(uint8_t data) 
{
    uint8_t next_head = (buffer_tx_head + 1) % BUFFER_TX_SIZE;
    if(next_head == buffer_tx_tail) {return;}  // error buffer is full

    buffer_tx[buffer_tx_head] = data;
    buffer_tx_head = next_head;

}

void usartFlush()
{
	
	while(buffer_tx_tail==buffer_tx_head)// test if tx buffer empty
	{
      
		while ( !( UCSR1A & (1<<UDRE1)) ); 	// Wait for empty transmit buffer
		UDR1 = buffer_tx[buffer_tx_tail];// Put data into buffer, sends the data
		
		buffer_rx_tail = (buffer_rx_tail + 1) % BUFFER_TX_SIZE_SIZE; // Move tail forward
	}
	return;
}

