#include "tasks/task1.h"
#include "sysCall/sysCall.h"
#include "sysCall/gpio.h"
#include "services/msg.h"

uint8_t task1_msg_channel;

void task1(void)
{

	if( msgRequestChannel(&task1_msg_channel) == ERR_NO_ERROR )
	{
		msgWritreText(task1_msg_channel, "[task1] hello !\n", MSG_TO_USART);
	}

	while( 1 )
	{
		gpioSignalToggle(GPIO_SIGNAL_TASK1_LED);

		sysCallSetThreadRTC(50);
		while( sysCallGetThreadRTC() > 0 );
	}
}
