#include <avr/io.h>
#include "sysCall/TaskMate_public.h"
#include "tasks/task2.h"

void task2(void)
{

	while( 1 )
	{
		gpioSignalToggle(GPIO_SIGNAL_TASK2_LED);

		sysCallSetThreadRTC(50);
		while( sysCallGetThreadRTC() > 0 );
	}
}
