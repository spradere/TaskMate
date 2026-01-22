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
 * @file option.h
 * @brief autoCode option parser header
 *
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include "autoCode.h"

// autoCode option structure
typedef struct
{
	const char * const arch_name;
	const char * const mcu_name;
	const char * const board_name;
} auto_options_t;

#endif
