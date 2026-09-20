/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file stack.c
 * @brief Stack command implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "stack.h"

#include "interfaces/tm_define.h"
#include "system/sysCall/sc_threads.h"
#include "tmLibc/tm_string.h"
#include "tmLibc/tm_syslog.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

bool stack(uint8_t argc, char *argv[])
{
	if( (argc != 2) ||
		(tm_strncmp(TM_STR_RAM(argv[1]), TM_STR("depth"), TM_STRING_SIZE_MAX) != 0) )
	{
		tm_syslog(TM_STR("[stack] usage:\n"));
		tm_syslog(TM_STR("\tstack depth\n"));
		return false;
	}

	tm_syslog(TM_STR("[stack] depth:\n"));
	const uint16_t thread_count = sc_threadGetCount();
	for( uint16_t id = 0; id < thread_count; id++ )
	{
		const tm_string_t *name;
		uint8_t run_level;
		uint16_t depth_bytes;
		if( !sc_threadGetInfo(id, &name, &run_level) ||
			!sc_threadGetStackDepth(id, &depth_bytes) )
		{
			return false;
		}

		(void)run_level;
		tm_syslog(TM_STR("\t%s depth=%u bytes\n"), name, depth_bytes);
	}

	return true;
}
