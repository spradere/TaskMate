#ifndef UART0_4_H
#define UART0_4_H


#define F_CPU 16000000UL  // CPU Frequency (adjust as needed)
#define BAUD 9600         // Desired Baud Rate
#define UBRR_VALUE ((F_CPU / (16UL * BAUD)) - 1)  // Baud Rate Calculation

void uart(void);

void bufferPush(uint8_t);
uint8_t bufferPop(void);

#endif
