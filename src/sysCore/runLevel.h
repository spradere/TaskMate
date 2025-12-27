/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file runLevel.h
 * @brief run level header
 *
 */

#ifndef RUNLEVEL_H
#define RUNLEVEL_H

#include <stdint.h>

// run levels definitions
#define RUN_NONE 0 /**< no start, could be started later via sCLI */
#define RUN_CORE 1 /**< start minimal critical system compoments */
#define RUN_DRIVER 2 /**< start hardware drivers */
#define RUN_SERVICE 3 /**< start system services */
#define RUN_USER 4 /**< start user tasks*/

#define RUN_LEVEL_MASK 0x07 /**< run leval mask in module status */
#define RUN_LEVEL_COUNT 5

// [autoCode_tag] run_levels alloc
#define RUN_LEVEL0_THREADS_COUNT 0
#define RUN_LEVEL1_THREADS_COUNT 0
#define RUN_LEVEL2_THREADS_COUNT 0
#define RUN_LEVEL3_THREADS_COUNT 2
#define RUN_LEVEL4_THREADS_COUNT 4

typedef struct
{
	uint8_t level0[RUN_LEVEL0_THREADS_COUNT + 1];
	uint8_t level1[RUN_LEVEL1_THREADS_COUNT + 1];
	uint8_t level2[RUN_LEVEL2_THREADS_COUNT + 1];
	uint8_t level3[RUN_LEVEL3_THREADS_COUNT + 1];
	uint8_t level4[RUN_LEVEL4_THREADS_COUNT + 1];
	uint8_t *levels[5];
	uint8_t current;
	uint8_t next;
} run_levels_t;

// [/tag]

void runLevelInit(void);

#endif
