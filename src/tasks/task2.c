#include "tasks/task2.h"
#include <avr/io.h>
#include "sysCall/sysCall.h"
#include "sysCall/gpio.h"

void task2(void)
{

	while( 1 )
	{
		gpioSignalToggle(GPIO_SIGNAL_TASK2_LED);

		sysCallSetThreadRTC(50);
		while( sysCallGetThreadRTC() > 0 );
	}
}
