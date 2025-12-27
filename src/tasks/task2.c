#include "tasks/task2.h"
#include "sysCall/sysCall.h"
#include "sysCall/gpio.h"
#include "services/msg.h"

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

		sysCallSetThreadRTC(50);
		while( sysCallGetThreadRTC() > 0 );
	}
}
