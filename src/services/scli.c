/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file scli.c
 * @brief implementation of serial command line interface
 *
 */

#include "services/scli.h"

#include <stdint.h>

#include "hal/public/hal_usart.h"
#include "services/msg.h"
#include "sysCall/sysCall.h"
#include "tm_libc/tm_syslog.h"

static uint8_t scli_msg_channel;

static void scliEcho(void);

void scli(void)
{

	if( msgRequestChannel(&scli_msg_channel) == ERR_NO_ERROR )
	{
		msgWritreText(scli_msg_channel, "[scli] ready to work\n", MSG_TO_USART);
	}

	while( 1 )
	{
		scliEcho(); // Echo echo echo echo echo echo echo

		sc_threadSetSTC(100);
		while( sc_threadGetSTC() > 0 ) { sc_handYield(); };
	}
}

static void scliEcho(void)
{
	uint8_t data;

	if( hal_usartTestBufferRx() != ERR_HAL_USART_RX_BUFFER_EMPTY )
	{
		uint8_t i = 0;
		char line[128];

		// fail ! msgWritreText(scli_msg_channel, "[scli] receive :", MSG_TO_USART);

		while( (hal_usartRead(&data) != ERR_HAL_USART_RX_BUFFER_EMPTY) && (i < (sizeof(line) - 1)) )
		{
			line[i++] = (char)data;
		}
		line[i] = 0;

		msgWritreText(scli_msg_channel, line, MSG_TO_USART);
	}
}
