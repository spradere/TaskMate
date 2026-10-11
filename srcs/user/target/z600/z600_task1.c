/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file z600_task1.c
 * @brief First simulation task implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "z600_task1.h"

#include "system/sysCall/sc_threads.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define Z600TASK1_DELAY_TICKS 50U

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void task1(void)
{
	sc_threadSetInitialized();

	while( 1U )
	{
		sc_threadSetSTC(Z600TASK1_DELAY_TICKS);
		while( sc_threadGetSTC() > 0U );
	}
}
