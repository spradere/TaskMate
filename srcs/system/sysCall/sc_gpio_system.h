/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sc_gpio_system.h
 * @brief System-service-only GPIO initialization declarations.
 */

#ifndef SYSCALL_SC_GPIO_SYSTEM_H
#define SYSCALL_SC_GPIO_SYSTEM_H

#if !defined(SC_GPIO_SYSTEM_ALLOWED)
	#error "NOT ALLOWED INCLUDE : sc_gpio_system.h"
#endif

/* ============================================================================
 * Public API
 * ========================================================================== */

void sc_gpio_signalsInit(void);

#endif // SYSCALL_SC_GPIO_SYSTEM_H
