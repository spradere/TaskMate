/*
 * TaskMate Project
 * (c) 2025 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the TaskMate License v1.0.
 * See the LICENSE file for full license terms.
 *
 * Non-commercial use permitted under conditions. Commercial use requires a separate license.
 * Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
 *
 * Powered by TaskMate, (c) 2025 PRADERE Sebastien
 */

/**
 * @file writeInclude.h
 * @brief write include code header
 *
 * @todo nothing
 */

#ifndef WRITE_INCLUDE_H
#define WRITE_INCLUDE_H

#include "utility/autoCode_src/autoCode.h"

#define INCLUDE_SYSTEM_PART 1
#define INCLUDE_HAL_PART 2

void writeInclude(const modules_database_t *data_base, const int type ,const char *file_name,
		const char *arch, const char *mcu, const char *board);

#endif
