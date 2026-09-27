/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file scli_thread.h
 * @brief thread command declarations.
 */

#ifndef COMMANDS_SCLI_THREAD_H
#define COMMANDS_SCLI_THREAD_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include <stdbool.h>
#include <stdint.h>

/* ============================================================================
 * Public API
 * ========================================================================== */

bool threadCommand(uint8_t argc, char *argv[]);

#endif // COMMANDS_SCLI_THREAD_H
