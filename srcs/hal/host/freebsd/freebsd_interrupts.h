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

#ifndef HAL_HOST_FREEBSD_FREEBSD_INTERRUPTS_H
#define HAL_HOST_FREEBSD_FREEBSD_INTERRUPTS_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <signal.h>

/* ============================================================================
 * Private host definitions
 * ========================================================================== */

#define FREEBSD_SIGNAL_SCHED SIGVTALRM
#define FREEBSD_SIGNAL_STC SIGALRM

/* ============================================================================
 * Private host API
 * ========================================================================== */

bool freebsd_interruptsMask(sigset_t *mask);
bool freebsd_interruptsBlock(sigset_t *previous);

#endif // HAL_HOST_FREEBSD_FREEBSD_INTERRUPTS_H
