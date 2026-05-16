/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file macros.h
 * @brief TaskMate general macro.
 *
 */

#ifndef INTERFACES_MACROS_H
#define INTERFACES_MACROS_H

// general system macros
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

// string macros
#define CONCAT2(a, b) a##b
#define CONCAT(a, b) CONCAT2(a, b)
#define STRING2(x) #x
#define INT_TO_STRING(x) STRING2(x)

// store source file name in ROM
#define TM_STORE_FILE_NAME(name) TM_STR_ROM_NEW(name, __FILE_NAME__)

// generate unique name
#define UNIQUE_NAME(prefix) CONCAT(prefix, __LINE__)

#endif // INTERFACES_MACROS_H
