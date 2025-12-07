#include <avr/io.h>
#include "sysCall/TaskMate_public.h"
#include "tasks/task1.h"

void task1(void)
{

	while( 1 )
	{
		gpioSignalToggle(GPIO_SIGNAL_TASK1_LED);

		sysCallSetThreadRTC(50);
		while( sysCallGetThreadRTC() > 0 );
	}
}
