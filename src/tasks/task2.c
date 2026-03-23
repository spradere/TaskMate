#include "tasks/task2.h"

#include "services/msg.h"
#include "sysCall/gpio.h"
#include "sysCall/sysCall.h"
#include "sysCall/panic.h"
#include "tm_libc/tm_stdio.h"

uint8_t task2_msg_channel;

void task2(void)
{

	if( msgRequestChannel(&task2_msg_channel) == ERR_NO_ERROR )
	{
		msgWritreText(task2_msg_channel, "[task2] task2 here !\n", MSG_TO_USART);
	}


	while( 1 )
	{
		gpio_signalToggle(GPIO_SIGNAL_TASK2_LED);

		sc_threadSetSTC(50);
		while( sc_threadGetSTC() > 0 );

	}
}
