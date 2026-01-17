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
 * @file hal_tm_options.h
 * @brief arch avr8 definitions
 *
 */

// libc selection
#define TM_LIBC_TASKMATE 1
#define TM_LIBC_CSTD 0

#if !(TM_LIBC_CSTD ^ TM_LIBC_TASKMATE)
_Static_assert(0, "Select exactly one libc, external or internal");
#endif
