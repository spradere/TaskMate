/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file scli.c
 * @brief implementation of serial command line interface
 *
 */

#include "services/scli.h"

#include <stdint.h>

#include "hal/auto_hal_user.h"
#include "services/msg.h"
#include "sysCall/sysCall.h"

uint8_t scli_msg_channel;

void scli(void)
{
	if( msgRequestChannel(&scli_msg_channel) == ERR_NO_ERROR )
	{
		msgWritreText(scli_msg_channel, "[scli] ready to work\n", MSG_TO_USART);
	}

	while( 1 )
	{
		scliEcho(); // Echo echo echo echo echo echo echo

		sysCallSetThreadRTC(100);
		while( sysCallGetThreadRTC() > 0 ) { sysCallYieldHand(); };
	}
}

void scliEcho(void)
{
	uint8_t data;
	char line[128];
	uint8_t i;

	if( hal_usartTestBufferRx() != ERR_HAL_USART_RX_BUFFER_EMPTY )
	{
		i = 0;
		// msgWritreText(scli_msg_channel, "[scli] recive :", MSG_TO_USART);
		//   todo add a test to ensure message was proceed
		//   or snprintf return writed charter number

		while( (hal_usartRead(&data) != ERR_HAL_USART_RX_BUFFER_EMPTY) && (i < (sizeof(line) - 1)) )
		{
			line[i++] = (char)data;
		}
		line[i] = 0;

		msgWritreText(scli_msg_channel, line, MSG_TO_USART);
	}
}
