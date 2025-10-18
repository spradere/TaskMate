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
 * @file msg.c
 * @brief implementation of message display server
 *
 * @todo all
 */

#include <avr/io.h>

#include "sysCore/TaskMate_public.h"
#include "services/msg.h"
#include "libc/string.h"

// Send message to :
#include "drivers/lcdAMC2004.h"
#include "drivers/usart1.h"

// variables
channel_item_t channels[MSG_CHANNELS_MAX];

void msg(void)
{
	// init channels
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		channels[i].status = 0;
		channels[i].text[0] = 0;
	}

	// run

	uint8_t channel;

	if( msgRequestChannel(&channel) == ERR_SUCCESS )
	{
		msgWritreText(channel, "msg : Essai USART1 \n", MSG_TO_USART1);
	}

	if( msgRequestChannel(&channel) == ERR_SUCCESS ) { msgWritreText(channel, "\3msg : LCD test ok", MSG_TO_LCD); }

	msgProcess();

	while( 1 )
	{
		sysCallSetThreadTC(10);
		while( sysCallGetThreadTC() > 0 ) { sysCallYieldHand(); };
	}
}

errorCode_t msgRequestChannel(uint8_t *channel)
{
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		if( (channels[i].status & (1 << MSG_FLAG_IN_USE)) == 0 )
		{
			channels[i].status |= (1 << MSG_FLAG_IN_USE);
			*channel = i;
			return ERR_SUCCESS;
		}
	}
	return ERR_MSG_OUT_OF_FREE_CHANNEL;
}

void msgWritreText(uint8_t channel, const char *msg, uint8_t dest)
{
	strncpy(channels[channel].text, MSG_SIZE_MAX, msg);

	channels[channel].status &= ~(1 << MSG_TO_MASK);
	channels[channel].status |= dest;
	channels[channel].status |= (1 << MSG_FLAG_SEND);
}

void msgProcess(void)
{
	for( uint8_t channel = 0; channel < MSG_CHANNELS_MAX; channel++ )
	{
		if( (channels[channel].status & (1 << MSG_FLAG_SEND)) != 0 )
		{
			switch( channels[channel].status & MSG_TO_MASK )
			{
				case MSG_TO_LCD:

					lcdAMC2004SetCursor(channels[channel].text[0], 0);
					// Zap escape code for LCD line select
					uint8_t i_src, i_dest = 0;
					for( i_src = 1; channels[channel].text[i_dest] != 0; i_src++ )
					{
						channels[channel].text[i_dest++] = channels[channel].text[i_src];
					}
					channels[channel].text[i_dest] = 0;

					lcdAMC2004WriteString(channels[channel].text);
					break;

				case MSG_TO_USART1:

					usart1WriteString(channels[channel].text);
					usart1SendTXBuffer();
					break;

				case MSG_TO_NULL:
					break;

				default:
					usart1WriteString("[msg.c 118] error unknow destination\n");
					usart1SendTXBuffer();
			}
			channels[channel].status &= ~(1 << MSG_FLAG_IN_USE);
		}
	}
}
