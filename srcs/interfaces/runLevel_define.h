/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file runLevel_define.h
 * @brief interfaces run level header declarations.
 */

#ifndef INTERFACES_RUNLEVEL_DEFINE_H
#define INTERFACES_RUNLEVEL_DEFINE_H

#if !defined(TM_SYSTEM_CRITICAL_ALLOWED)
	#error "NOT ALLOWED INCLUDE : runLevel_define.h"
#endif

// run levels definitions
#define RUN_NONE 0
#define RUN_CORE 1
#define RUN_DRIVER 2
#define RUN_SERVICE 3
#define RUN_USER 4

#define RUN_LEVEL_MASK 0x07
#define RUN_LEVEL_COUNT 5

#endif
