#include <avr/io.h>

#include "sysCore/TaskMate_define.h"

static uint8_t timer3_status=DRIVER_UNLOCK;
static uint8_t timer3_who;

uint8_t timer3Init(void)
{
	if(timer3_status==DRIVER_UNLOCK)
	{
		// Set up timer3 interrupt for RTC
		TCCR3B |= (1 << WGM32) | (1 << CS32); // CTC mode, prescaler 256
		OCR3A = 624; // Interrupt every 10ms
		TIMSK3 |= (1 << OCIE3A);	
		
		return DRIVER_INIT;
	}
	else {return DRIVER_LOCK;}
}


uint8_t timer3Lock(uint8_t by)
{
	timer3_status=DRIVER_LOCK;
	timer3_who=by;
	return 0;

}
