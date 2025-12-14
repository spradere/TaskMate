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

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Auto generated code, do not edit !
// any changes will be lost
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifndef AUTOALLOC_H
#define AUTOALLOC_H

#include <stdint.h>

#define DRIVERS_COUNT 6
#define THREADS_COUNT 4

#define RUN_LEVEL0_THREADS_COUNT 0
#define RUN_LEVEL1_THREADS_COUNT 0
#define RUN_LEVEL2_THREADS_COUNT 0
#define RUN_LEVEL3_THREADS_COUNT 2
#define RUN_LEVEL4_THREADS_COUNT 4

typedef struct
{
	uint16_t level0[RUN_LEVEL0_THREADS_COUNT + 1];
	uint16_t level1[RUN_LEVEL1_THREADS_COUNT + 1];
	uint16_t level2[RUN_LEVEL2_THREADS_COUNT + 1];
	uint16_t level3[RUN_LEVEL3_THREADS_COUNT + 1];
	uint16_t level4[RUN_LEVEL4_THREADS_COUNT + 1];
	uint16_t *levels[5];
	uint8_t current;
	uint8_t next;
} run_levels_t;

#endif
