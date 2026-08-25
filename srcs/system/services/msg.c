/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file msg.c
 * @brief msg implementation.
 */

#include "msg.h"

#include "interfaces/macros.h"
#include "system/sysCall/sysCall.h"
#include "tm_libc/tm_stdio.h"
#include "tm_libc/tm_string.h"

// Send message to :
#include "hal/public/lcd.h"
#include "hal/public/usart.h"

// private message channel internals
#define MSG_FLAG_IN_USE 3
#define MSG_FLAG_SEND 4

#define MSG_CHANNELS_MAX 4
#define MSG_SIZE_MAX 64

typedef struct
{
	uint8_t status;
	char text[MSG_SIZE_MAX];
} channel_item_t;

// variables
static channel_item_t channels[MSG_CHANNELS_MAX];

static void msgProcess(void);

void msg(void)
{
	// init channels
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		channels[i].status = 0;
		channels[i].text[0] = 0;
	}

	// run

	uint8_t chan;

	if( msgRequestChannel(&chan) == ERR_NO_ERROR )
	{
		msgWriteText(chan, TM_STR("[msg] canal test to USART 110\n"), MSG_TO_USART);
	}

	msgProcess();
	msgFreeChannel(chan);

	if( msgRequestChannel(&chan) == ERR_NO_ERROR )
	{
		msgWriteText(chan, TM_STR("\3HEX hex 16^16"), MSG_TO_LCD);
	}

	msgProcess();
	msgFreeChannel(chan);

	while( 1 )
	{
		msgProcess();

		sc_threadSetSTC(100);
		while( sc_threadGetSTC() > 0 ) { sc_coopYield(); };
	}
}

err_codes_t msgRequestChannel(uint8_t *channel)
{
	for( uint8_t i = 0; i < MSG_CHANNELS_MAX; i++ )
	{
		if( (channels[i].status & (uint8_t)(1u << MSG_FLAG_IN_USE)) == 0 )
		{
			TM_SETBIT(channels[i].status, MSG_FLAG_IN_USE);
			*channel = i;
			return ERR_NO_ERROR;
		}
	}
	return ERR_MSG_OUT_OF_FREE_CHANNEL;
}

void msgFreeChannel(uint8_t channel) { TM_CLEARBIT(channels[channel].status, MSG_FLAG_IN_USE); }

void msgWriteText(uint8_t channel, tm_string_t msg, uint8_t dest)
{
	tm_strncpy(channels[channel].text, msg, MSG_SIZE_MAX);

	channels[channel].status &= (uint8_t)~MSG_TO_MASK;
	channels[channel].status |= dest;
	TM_SETBIT(channels[channel].status, MSG_FLAG_SEND);
}

static void msgProcess(void)
{
	for( uint8_t channel = 0; channel < MSG_CHANNELS_MAX; channel++ )
	{
		if( ((channels[channel].status & (1 << MSG_FLAG_SEND)) != 0) &&
			((channels[channel].status & (1 << MSG_FLAG_IN_USE)) != 0) )
		{
			switch( channels[channel].status & MSG_TO_MASK )
			{
				case MSG_TO_LCD:

					hal_lcdSetCursor((uint8_t)channels[channel].text[0], 0);
					uint8_t i_src, i_dest = 0;
					// i_src = 1 zap escape code for LCD line select
					for( i_src = 1; channels[channel].text[i_dest] != 0; i_src++ )
					{
						channels[channel].text[i_dest++] = channels[channel].text[i_src];
					}
					channels[channel].text[i_dest] = 0;

					hal_lcdWriteString(TM_STR_RAM(channels[channel].text));
					break;

				case MSG_TO_USART:

					hal_usartWriteString(TM_STR_RAM(channels[channel].text));
					hal_usartSendTXBuffer();
					TM_CLEARBIT(channels[channel].status, MSG_FLAG_SEND);
					break;

				case MSG_TO_NULL:
					break;

				default:
					hal_usartWriteString(TM_STR("[msg.c:118] error unknown destination\n"));
					hal_usartSendTXBuffer();
			}
		}
	}
}
