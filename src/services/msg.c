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

	uint8_t ch_id;

	if( msgRequestChannel(&ch_id) == ERR_SUCCESS )
	{
		msgWritreText(ch_id, "msg : Essai USART1 \n", MSG_TO_USART1);
	}

	if( msgRequestChannel(&ch_id) == ERR_SUCCESS ) { msgWritreText(ch_id, "\3msg : Essai LCD", MSG_TO_LCD); }

	msgProcess();

	while( 1 )
	{
		sysCallSetThreadTC(10);
		while( sysCallGetThreadTC() > 0 ) { sysCallYieldHand(); };
	}
}

errorCode_t msgRequestChannel(uint8_t *channel_id)
{
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		if( (channels[i].status & (1 << MSG_FLAG_IN_USE)) == 0 )
		{
			channels[i].status |= (1 << MSG_FLAG_IN_USE);
			*channel_id = i;
			return ERR_SUCCESS;
		}
	}
	return ERR_MSG_OUT_OF_FREE_CHANNEL;
}

void msgWritreText(uint8_t channel_id, const char *msg, uint8_t dest)
{
	strncpy(channels[channel_id].text, msg, MSG_SIZE_MAX);

	channels[channel_id].status &= ~(1 << MSG_TO_MASK);
	channels[channel_id].status |= dest;
	channels[channel_id].status |= (1 << MSG_FLAG_SEND);
}

void msgProcess(void)
{
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		if( (channels[i].status & (1 << MSG_FLAG_SEND)) != 0 )
		{
			switch( channels[i].status & MSG_TO_MASK )
			{
				case MSG_TO_LCD:

					lcdAMC2004SetCursor(channels[i].text[0], 0);
					// Zap escape code for line select
					uint8_t i_src, i_dest = 0;
					for( i_src = 1; channels[i].text[i_dest] != 0; i_src++ )
					{
						channels[i].text[i_dest++] = channels[i].text[i_src];
					}

					lcdAMC2004WriteString(channels[i].text);
					break;

				case MSG_TO_USART1:

					usart1WriteString(channels[i].text);
					usart1SendTXBuffer();
					break;

				case MSG_TO_NULL:
					break;

				default:
					usart1WriteString("[msg.c 118] error unknow destination\n");
					usart1SendTXBuffer();
			}
			channels[i].status &= ~(1 << MSG_FLAG_IN_USE);
		}
	}
}
