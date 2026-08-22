/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli.c
 * @brief scli implementation.
 *
 */

#include "scli.h"

#include <stdint.h>

#include "hal/public/usart.h"
#include "system/services/msg.h"
#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_syslog.h"

static uint8_t scli_msg_channel;

static void scliEcho(void);

void scli(void)
{

	if( msgRequestChannel(&scli_msg_channel) == ERR_NO_ERROR )
	{
		msgWriteText(scli_msg_channel, "[scli] ready to work\n", MSG_TO_USART);
	}

	while( 1 )
	{
		scliEcho(); // Echo echo echo echo echo echo echo

		sc_threadSetSTC(100);
		while( sc_threadGetSTC() > 0 ) { sc_coopYield(); };
	}
}

static void scliEcho(void)
{
	uint8_t data;

	if( hal_usartTestBufferRx() != ERR_HAL_USART_RX_BUFFER_EMPTY )
	{
		uint8_t i = 0;
		char line[128];

		while( (hal_usartRead(&data) == ERR_NO_ERROR) && (i < (sizeof(line) - 1)) )
		{
			line[i++] = (char)data;
		}
		line[i] = 0;

		msgWriteText(scli_msg_channel, line, MSG_TO_USART);
	}
}
