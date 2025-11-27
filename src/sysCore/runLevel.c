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
 * @file runLevel.c
 * @brief run level code
 *
 */

#include "sysCore/autoAlloc.h" // get run_levels_t
#include "sysCore/runLevel.h"

static run_levels_t to_run;

// NOLINTBEGIN
// NOLINT(readability-magic-numbers)

void runLevelInit(void)
{
	// do not edit code between tag : automatic generated code by autoCode
	// [autoCode_tag] run levels
	to_run = (run_levels_t){
		.level0 = {0},
		.level1 = {0},
		.level2 = {0},
		.level3 = {2,0,1},
		.level4 = {4,0,1,2,3},
		.levels = {to_run.level0, to_run.level1, to_run.level2, to_run.level3, to_run.level4}
	};
	to_run.current=RUN_CORE;
	to_run.next=RUN_CORE;
	// [/tag]
}

// NOLINTEND
