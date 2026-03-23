#include "tasks/task1.h"

#include "services/msg.h"
#include "sysCall/gpio.h"
#include "sysCall/sysCall.h"
#include "sysCall/panic.h"
#include "tm_libc/tm_stdio.h"


uint8_t task1_msg_channel;

void task1(void)
{

	if( msgRequestChannel(&task1_msg_channel) == ERR_NO_ERROR )
	{
		msgWritreText(task1_msg_channel, "[task1] hello !\n", MSG_TO_USART);
	}


	while( 1 )
	{

		gpio_signalToggle(GPIO_SIGNAL_TASK1_LED);

		sc_threadSetSTC(50);
		while( sc_threadGetSTC() > 0 );
	}
}
