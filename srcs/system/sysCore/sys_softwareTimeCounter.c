/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file sys_softwareTimeCounter.c
 * @brief tm software time counter implementation.
 *
 */

/* =============================================================================
 * Declarations - Include
 * ===========================================================================*/

#include "sys_softwareTimeCounter.h"

#include "interfaces/drv_timerSTC.h"
#include "system/sysCore/sys_threads.h"

/* -----------------------------------------------
 * Private function prototypes
 * ---------------------------------------------*/

static void tm_softwareTimeCounter(void);

/* =============================================================================
 * Implementation - Functions
 * ===========================================================================*/

void tm_softwareTimeCounterInit(void) { hal_timerSTCSetCallback(tm_softwareTimeCounter); }

void tm_softwareTimeCounter(void) { mod_threadsTickSTC(); }
