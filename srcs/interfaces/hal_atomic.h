/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_atomic.h
 * @brief Hardware-independent atomic section contract.
 */

#ifndef INTERFACES_HAL_ATOMIC_H
#define INTERFACES_HAL_ATOMIC_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdint.h>

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/* Opaque token carrying the interrupt state captured on atomic entry. */
typedef uintptr_t hal_atomic_state_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * Capture the complete interrupt state and mask interrupts without an intervening window.
 * Nested calls and calls from interrupt context preserve the state seen by each caller.
 *
 * @return State token to pass unchanged to the matching hal_atomicEnd().
 */
hal_atomic_state_t hal_atomicStart(void);

/** Restore exactly the interrupt state captured by the matching hal_atomicStart(). */
void hal_atomicEnd(hal_atomic_state_t state);

#endif // INTERFACES_HAL_ATOMIC_H
