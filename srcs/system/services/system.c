/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file system.c
 * @brief system management implementation.
 */

#include "system.h"

#include "system/sysCall/sysCall.h"

void system(void)
{

	while( 1 )
	{
		sc_threadSetSTC(100);
		while( sc_threadGetSTC() > 0 ) { sc_coopYield(); };
	}
}

