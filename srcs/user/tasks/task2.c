/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file task2.c
 * @brief tasks task2 implementation.
 *
 */

#include "user/tasks/task2.h"

#include "system/services/msg.h"
#include "system/sysCall/sc_gpio.h"
#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_stdio.h"

uint8_t task2_msg_channel;

void task2(void)
{

	if( msgRequestChannel(&task2_msg_channel) == ERR_NO_ERROR )
	{
		msgWriteText(task2_msg_channel, "[task2] task2 here !\n", MSG_TO_USART);
	}

	while( 1 )
	{
		sc_gpio_signalToggle(GPIO_SIGNAL_TASK2_LED);

		sc_threadSetSTC(50);
		while( sc_threadGetSTC() > 0 );
	}
}
