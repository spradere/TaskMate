
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
 * @file error.c
 * @brief System wide error implementation
 *
 * @todo Timeout routine, critical/fatal errors handling
 */

#include "sysCall/error.h"

// do not edit code between tag : automatic generated code by autoCode
// [autoCode_tag] error catalog
const error_item_t error_catalog[] = {{"No error", 1},			 {"TX buffer is empty", 1},
									  {"RX buffer is empty", 1}, {"TX buffer is full", 1},
									  {"RX buffer is full", 1},	 {"No more free channels", 1}};
// [/tag]
