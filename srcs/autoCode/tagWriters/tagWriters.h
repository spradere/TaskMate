/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file tagWriters.h
 * @brief Shared autoCode tag writer declarations.
 */

#ifndef TAGWRITERS_TAGWRITERS_H
#define TAGWRITERS_TAGWRITERS_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "../globalError.h"
#include "../options.h"

/* =============================================================================
 * Public definitions
 * ===========================================================================*/

/**
 * @brief Provides inputs and error state to a generated tag writer.
 */
typedef struct
{
	const modules_database_t *data_base;
	FILE *file;
	const error_catalog_t *errors;
	const options_list_t *auto_options;
	bool *file_error;
} tag_writer_context_t;

/* =============================================================================
 * Public API
 * ===========================================================================*/

/**
 * @brief Write generated module counts.
 */
void tagWriterWriteModulesCount(const tag_writer_context_t *context);
/**
 * @brief Write generated thread declarations.
 */
void tagWriterWriteThreadsList(const tag_writer_context_t *context);
/**
 * @brief Write generated driver declarations.
 */
void tagWriterWriteDriversList(const tag_writer_context_t *context);
/**
 * @brief Write generated thread stack definitions.
 */
void tagWriterWriteThreadStacks(const tag_writer_context_t *context);
/**
 * @brief Write generated thread allocation data.
 */
void tagWriterWriteThreadsAlloc(const tag_writer_context_t *context);
/**
 * @brief Write generated thread names.
 */
void tagWriterWriteThreadNameCatalog(const tag_writer_context_t *context);
/**
 * @brief Write generated driver allocation data.
 */
void tagWriterWriteDriversAlloc(const tag_writer_context_t *context);
/**
 * @brief Write generated driver names.
 */
void tagWriterWriteDriverNameCatalog(const tag_writer_context_t *context);
/**
 * @brief Write generated driver capability flags.
 */
void tagWriterWriteDriverHave(const tag_writer_context_t *context);
/**
 * @brief Write generated error identifiers.
 */
void tagWriterWriteErrorEnum(const tag_writer_context_t *context);
/**
 * @brief Write generated error messages.
 */
void tagWriterWriteErrorCatalog(const tag_writer_context_t *context);
/**
 * @brief Write generated logical GPIO signals.
 */
void tagWriterWriteGpioSignals(const tag_writer_context_t *context);
/**
 * @brief Write generated GPIO wiring.
 */
void tagWriterWriteWireGpio(const tag_writer_context_t *context);
/**
 * @brief Write generated SCLI command data.
 */
void tagWriterWriteScliCommands(const tag_writer_context_t *context);

#endif // TAGWRITERS_TAGWRITERS_H
