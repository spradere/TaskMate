/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file hal_boardInit.h
 * @brief board init arduino mega implematation
 *
 */

#ifndef HAL_BOARDINIT_H
#define HAL_BOARDINIT_H

#ifndef HAL_SYSTEM_CRITICAL_ALLOWED
	#error "hal system critical not allowed"
#endif

void hal_boardInit(void);

#endif
