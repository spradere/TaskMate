/*
 * TaskMate Project
 * (c) 2026 PRADERE Sebastien
 *
 * This file is part of TaskMate and is distributed under the BSD-2-Clause License.
 * See the LICENSE file for full license terms.
 */

/**
 * @file rtc.h
 * @brief public hal_rtc_ZS042 header declarations.
 *
 */

#ifndef PUBLIC_RTC_H
#define PUBLIC_RTC_H

#if defined(HWT_test1)
	#include "hal/drivers/rtc_ZS_042/rtc_ZS042.h"
	#define HAL_RTC
#endif

#if !defined(HAL_RTC)
	#error "No hal implementation for rtc on selected hardware target."
#endif
#endif
