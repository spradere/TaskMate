/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file hal_delay.h
 * @brief Hardware-independent blocking delay contract.
 */

#ifndef INTERFACES_HAL_DELAY_H
#define INTERFACES_HAL_DELAY_H

#include <stdint.h>

/** Block the caller for at least the requested number of milliseconds. */
void hal_delayMs(uint16_t milliseconds);

/** Block the caller for at least the requested number of microseconds. */
void hal_delayUs(uint16_t microseconds);

#endif // INTERFACES_HAL_DELAY_H
