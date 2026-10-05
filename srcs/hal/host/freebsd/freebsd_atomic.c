/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_atomic.c
 * @brief FreeBSD atomic-section implementation.
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include <signal.h>
#include <stdint.h>

#include "freebsd_interrupts.h"
#include "interfaces/hal_atomic.h"
#include "interfaces/hal_halt.h"

/* -----------------------------------------------
 * Constants
 * ---------------------------------------------*/

#define FREEBSDATOMIC_MAX_NESTING 16U

/* -----------------------------------------------
 * Private variables
 * ---------------------------------------------*/

static sigset_t atomic_masks[FREEBSDATOMIC_MAX_NESTING];
static uint8_t atomic_depth;

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

hal_atomic_state_t hal_atomicStart(void)
{
	if( atomic_depth >= FREEBSDATOMIC_MAX_NESTING ) { hal_halt(); }
	if( !freebsd_interruptsBlock(&atomic_masks[atomic_depth]) ) { hal_halt(); }

	atomic_depth++;
	return (hal_atomic_state_t)atomic_depth;
}

void hal_atomicEnd(hal_atomic_state_t state)
{
	if( (state == 0U) || (state != atomic_depth) ) { hal_halt(); }

	atomic_depth--;
	if( sigprocmask(SIG_SETMASK, &atomic_masks[atomic_depth], 0U) != 0U ) { hal_halt(); }
}
