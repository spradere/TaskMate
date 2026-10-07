/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file freebsd_interrupts.h
 * @brief FreeBSD virtual-interrupt declarations.
 */

#ifndef FREEBSD_FREEBSD_INTERRUPTS_H
#define FREEBSD_FREEBSD_INTERRUPTS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <signal.h>
#include <stdbool.h>

/* ============================================================================
 * Private host definitions
 * ========================================================================== */

#define FREEBSD_SIGNAL_SCHED SIGVTALRM

/* ============================================================================
 * Private host API
 * ========================================================================== */

bool freebsd_interruptsMask(sigset_t *mask);
bool freebsd_interruptsBlock(sigset_t *previous);

#endif // FREEBSD_FREEBSD_INTERRUPTS_H
