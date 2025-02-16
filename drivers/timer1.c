#include <avr/io.h>

#include "sysCore/TaskMate_define.h"

static uint8_t timer1_status=DRIVER_UNLOCK;
static uint8_t timer1_who;


uint8_t timer1Init(void)
{	
	if(timer1_status==DRIVER_UNLOCK)
	{
		// Set up timer1 interrupt for scheduler
		TCCR1B |= (1 << WGM12) | (1 << CS11); // CTC mode, prescaler 8
		OCR1A = 1999; // Interrupt every 1ms
		TIMSK1 |= (1 << OCIE1A);
		// Set output for in board led 13
		LED_DDR |= (1 << LED_PIN);
	
		return DRIVER_INIT;
	}
	else {return DRIVER_LOCK;}
}

uint8_t timer1Lock(uint8_t by)
{
	timer1_status=DRIVER_LOCK;
	timer1_who=by;
	return 0;
}

