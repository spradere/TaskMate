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

#include <avr/io.h>
#include <stdint.h>
#include "sysCall/TaskMate_public.h"
#include "services/scli.h"
#include "hal/hal_user_api.h"

void scli(void)
{
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

	if( hal_usartTestBufferRx() != ERR_USART1_RX_BUFFER_EMPTY )
	{
		hal_usartWriteString("scli.c : ");

		while( hal_usartRead(&data) != ERR_USART1_RX_BUFFER_EMPTY )
		{
			if( hal_usartWriteChar(data) == ERR_USART1_TX_BUFFER_FULL ) { break; }
		}
		hal_usartSendTXBuffer();
	}
}
