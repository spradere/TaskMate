/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License
 * v1.0. See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a
 * separate license. Commercial licensing inquiries:
 * https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2026 PRADERE Sebastien
 */

/**
 * @file TaskMate.h
 * @brief TaskMate header declarations.
 *
 */

#ifndef TASKMATE_H
#define TASKMATE_H

// general system macro / def
#define NULL ((void *)0)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// string macros
#define STRING2(x) #x
#define INT_TO_STRING(x) STRING2(x)
#define TM_STORE_FILE_NAME(name) TM_STR_ROM_NEW(name, __FILE_NAME__)

// debug options
#define VERBOSE_LEVEL 0

// libc selection
#define TM_LIBC_TASKMATE 1
#define TM_LIBC_CSTD 0

#if !(TM_LIBC_CSTD ^ TM_LIBC_TASKMATE)
_Static_assert(0, "Select exactly one libc, external or internal");
#endif

#endif
