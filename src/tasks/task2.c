#include "tasks/task2.h"

#include "services/msg.h"
#include "sysCall/gpio.h"
#include "sysCall/sysCall.h"

uint8_t task2_msg_channel;

void task2(void)
{

	if( msgRequestChannel(&task2_msg_channel) == ERR_NO_ERROR )
	{
		msgWritreText(task2_msg_channel, "[task2] task2 here !\n", MSG_TO_USART);
	}

	while( 1 )
	{
		gpioSignalToggle(GPIO_SIGNAL_TASK2_LED);

		sysCallSetThreadSTC(50);
		while( sysCallGetThreadSTC() > 0 );
	}
}
