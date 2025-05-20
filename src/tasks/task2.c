#include <avr/io.h>
#include "sysCore/TaskMate_public.h"
#include "tasks/task2.h"

void task2(void)
{
	// Set PA1 as an output
	DDRA |= (1 << PA1);

	while( 1 )
	{
		PORTA ^= (1 << PA1);
		sysCallSetThreadTC(50);
		while( sysCallGetThreadTC() > 0 );
	}
}
