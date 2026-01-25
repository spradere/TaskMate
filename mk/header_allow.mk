################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2026 PRADERE Sebastien
#
################################################################################

################################################################################
# allowed source access system critical headers
################################################################################

CFLAGS_${SRC_DIR}/sysCore/TaskMate.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED
CFLAGS_${SRC_DIR}/sysCore/scheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED
CFLAGS_${SRC_DIR}/sysCore/initSys.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED
CFLAGS_${SRC_DIR}/sysCall/sysCall.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED
CFLAGS_${SRC_DIR}/hal/mcu/${MCU}/hal_timerScheduler.c = \
	-DHAL_SYSTEM_CRITICAL_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED

GREP_PATTERN1 = hal/auto_hal_system.h
GREP_ALLOWED1 = \
	${SRC_DIR}/hal/mcu/${MCU}/hal_timerScheduler.c \
	${SRC_DIR}/sysCore/TaskMate.c \
	${SRC_DIR}/sysCore/tm_scheduler.c \
	${SRC_DIR}/sysCore/modules.c \
	${SRC_DIR}/sysCall/sysCall.c \
	${SRC_DIR}/autoCode/autoCode.c

GREP_PATTERN2 = hal/auto_hal_init.h
GREP_ALLOWED2 = \
	${SRC_DIR}/sysCore/TaskMate.c \
	${SRC_DIR}/autoCode/autoCode.c

GREP_LIST != cat ${.PARSEDIR}/${.PARSEFILE} \
	| grep '^[[:space:]]*GREP_PATTERN[0-9][0-9]*[[:space:]]*=' \
	| sed 's/^[[:space:]]*GREP_PATTERN\([0-9]*\)[[:space:]]*=.*/\1/'
