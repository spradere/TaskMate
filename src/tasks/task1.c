#include <avr/io.h>
#include "sysCall/TaskMate_public.h"
#include "tasks/task1.h"

void task1(void)
{
	// Set PA0 as an output
	DDRA |= (1 << PA0);

	while( 1 )
	{
		PORTA ^= (1 << PA0);
		sysCallSetThreadRTC(50);
		while( sysCallGetThreadRTC() > 0 );
	}
}
