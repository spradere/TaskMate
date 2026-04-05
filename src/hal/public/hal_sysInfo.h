/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD 2-Clause License.
 * See the LICENSE file for full license terms.
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
