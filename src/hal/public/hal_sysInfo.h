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
 * @file hal_sysInfo.h
 * @brief public hal_sysInfo header declarations.
 *
 */

#ifndef HAL_SYSINFO_H
#define HAL_SYSINFO_H

#include <stdint.h>

#include "interfaces/tm_string_storage.h"

typedef struct
{
	const tm_string_t *tm_ver;
	const uint16_t tm_build;
	const tm_string_t *arch;
	const tm_string_t *mcu;
	const tm_string_t *board;
} hal_info_t;

void hal_sysInfoGet(const hal_info_t **dest);

#endif
