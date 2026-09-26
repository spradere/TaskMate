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

#ifndef AUTOCODE_TAGWRITERS_TAGWRITERS_H
#define AUTOCODE_TAGWRITERS_TAGWRITERS_H

/* =============================================================================
 * Includes
 * ===========================================================================*/

#include "../globalError.h"
#include "../options.h"

/* =============================================================================
 * Public definitions
 * ===========================================================================*/

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

void tagWriterWriteModulesCount(const tag_writer_context_t *context);
void tagWriterWriteModulesList(const tag_writer_context_t *context);
void tagWriterWriteThreadStacks(const tag_writer_context_t *context);
void tagWriterWriteThreadsAlloc(const tag_writer_context_t *context);
void tagWriterWriteThreadNameCatalog(const tag_writer_context_t *context);
void tagWriterWriteDriversAlloc(const tag_writer_context_t *context);
void tagWriterWriteDriverNameCatalog(const tag_writer_context_t *context);
void tagWriterWriteErrorEnum(const tag_writer_context_t *context);
void tagWriterWriteErrorCatalog(const tag_writer_context_t *context);
void tagWriterWriteGpioSignals(const tag_writer_context_t *context);
void tagWriterWriteWireGpio(const tag_writer_context_t *context);
void tagWriterWriteScliCommands(const tag_writer_context_t *context);

#endif // AUTOCODE_TAGWRITERS_TAGWRITERS_H
