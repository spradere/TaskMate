/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file fileUtility.h
 * @brief file utility header declarations.
 *
 */

#ifndef AUTOCODE_FILEUTILITY_H
#define AUTOCODE_FILEUTILITY_H

/* ============================================================================
 * Includes
 * ========================================================================== */

#include "autoCode.h"

/* ============================================================================
 * Public definitions
 * ========================================================================== */

/**
 * @name Managed file opening flags
 * @brief Select read-only access and optional missing files.
 * @{
 */
#define FILE_READONLY 1
#define FILE_MISSING_ALLOWED 2
/** @} */

/**
 * @brief Tracks a managed stream and ownership of its file name.
 */
typedef struct
{
	FILE *stream;
	bool stream_opened; // allow fclose()
	bool write_access; // report buffered write errors on close
	char *name; // usually borrowed; temporary file names belong to the registry
	bool name_allocated; // allow free()
} file_t;

/**
 * @brief Reports managed file operation results.
 */
typedef enum
{
	FILE_UTILITY_OK = 0,
	FILE_GET_LINE_SUCCESS = FILE_UTILITY_OK,
	FILE_GET_LINE_EOF,
	FILE_UTILITY_DIFFERENT,
	FILE_GET_LINE_ERROR,
	FILE_UTILITY_INVALID,
	FILE_UTILITY_OPEN,
	FILE_UTILITY_WRITE,
	FILE_UTILITY_CLOSE,
	FILE_UTILITY_SEEK,
	FILE_UTILITY_REMOVE,
	FILE_UTILITY_RENAME,
	FILE_UTILITY_ALLOC,
	FILE_UTILITY_REGISTER
} file_utility_err_t;

typedef file_utility_err_t file_get_line_result_t;

/* ============================================================================
 * Public API
 * ========================================================================== */

/**
 * @brief Report files changed during generation.
 */
void filePrintModified(void);
/**
 * @brief Publish changed temporary files over their targets.
 * @return FILE_UTILITY_OK on success; a file operation error otherwise.
 */
file_utility_err_t fileCmpReplaceAll(void);
/**
 * @brief Close a managed file and release its owned resources.
 * @param[in,out] file Managed file to close.
 * @return FILE_UTILITY_OK on success; a close or write error otherwise.
 */
file_utility_err_t fileClose(file_t *file);
/**
 * @brief Read the next bounded line from a managed file.
 * @param[in,out] file Managed input file.
 * @param[out] line Destination buffer.
 * @param line_size_max Destination capacity in bytes.
 * @return FILE_GET_LINE_SUCCESS, FILE_GET_LINE_EOF, or a read error.
 */
file_utility_err_t fileGetLine(file_t *file, char *line, size_t line_size_max);
/**
 * @brief Reset a managed file descriptor before use.
 */
void fileInit(file_t *file);
/**
 * @brief Open a managed file with the selected access mode.
 * @param[in,out] file Initialized file descriptor.
 * @param mode Standard C file opening mode.
 * @param special_mode Optional file mode flags.
 * @return FILE_UTILITY_OK on success; an open error otherwise.
 */
file_utility_err_t fileOpen(file_t *file, const char *mode, int special_mode);
/**
 * @brief Create a temporary file for a source path.
 * @param file_src_name Source path for the temporary file.
 * @param[out] file_tmp Receives the managed temporary file.
 * @return FILE_UTILITY_OK on success; a file operation error otherwise.
 */
file_utility_err_t fileMakeTmp(const char *file_src_name, file_t *file_tmp);
/**
 * @brief Get a diagnostic message for a file utility result.
 * @return Diagnostic text for the result.
 */
const char *fileUtilityErrorMessage(file_utility_err_t error);

#endif // AUTOCODE_FILEUTILITY_H
