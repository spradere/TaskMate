#include <avr/io.h>
#include "TaskMate0.4_public.h"
#include "uart0.4.h"
#include "error0.4.h"

#define BUFFER_SIZE 128

typedef struct
{
	uint8_t buffer[BUFFER_SIZE];
	uint8_t write_index=0;
	uint8_t read_index=0;
} Buffer;

Buffer uart_TX;
Buffer uart_RX;


void uart(void)
{
    // Set Baud Rate
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);  // High byte
    UBRR0L = (uint8_t)(UBRR_VALUE);       // Low byte

    // Enable Transmitter and Receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // Set Frame Format: 8 data bits, 1 stop bit, No parity (8N1)
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    while(1)
    {
		// read buffer for output
		
		// read uart for input
        
        // test purpose
        
        /*while (!(UCSR0A & (1 << RXC0)));  // Wait for data
        uint8_t data= UDR0;
        
		while (!(UCSR0A & (1 << UDRE0))); // Wait until TX buffer is empty
		UDR0 = data;  // Send byte*/
		
		uint8_t data;
		
        if ( (UCSR0A & (1 << RXC0))!=0 ) {data= UDR0;} // Wait for data
                
		if ( ((UCSR0A & (1 << UDRE0))!=0) && (data!=0) ){UDR0 = data;} // Wait until TX buffer is empty
		          
        //sysCallSetTaskRTC(100);
        //while(sysCallGetTaskRTC()>0);
    }

}
