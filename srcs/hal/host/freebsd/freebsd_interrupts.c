/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_interrupts.c
 * @brief FreeBSD virtual-interrupt implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "freebsd_interrupts.h"

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

bool freebsd_interruptsMask(sigset_t *mask)
{
	if( mask == 0 ) { return false; }
	if( sigemptyset(mask) != 0 ) { return false; }
	if( sigaddset(mask, FREEBSD_SIGNAL_SCHED) != 0 ) { return false; }
	if( sigaddset(mask, FREEBSD_SIGNAL_STC) != 0 ) { return false; }
	return true;
}

bool freebsd_interruptsBlock(sigset_t *previous)
{
	sigset_t mask;
	if( !freebsd_interruptsMask(&mask) ) { return false; }
	return sigprocmask(SIG_BLOCK, &mask, previous) == 0;
}
