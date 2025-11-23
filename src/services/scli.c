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
 * @todo Implement commands, before develop error handler
 */

#include <avr/io.h>
#include <stdint.h>
#include "sysCall/TaskMate_public.h"
#include "hal/board/arduino_mega/usart1.h"
#include "services/scli.h"

void scli(void)
{
	while( 1 )
	{
		scliEcho(); // Echo echo echo echo echo echo echo

		sysCallSetThreadTC(100);
		while( sysCallGetThreadTC() > 0 ) { sysCallYieldHand(); };
	}
}

void scliEcho(void)
{
	uint8_t data;

	if( usart1TestBufferRx() != ERR_USART1_RX_BUFFER_EMPTY )
	{
		usart1WriteString("scli.c : ");

		while( usart1Read(&data) != ERR_USART1_RX_BUFFER_EMPTY )
		{
			if( usart1WriteChar(data) == ERR_USART1_TX_BUFFER_FULL ) { break; }
		}
		usart1SendTXBuffer();
	}
}
