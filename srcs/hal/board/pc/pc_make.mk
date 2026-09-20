################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_BOARD_PC_PC_MAKE_MK
HAL_BOARD_PC_PC_MAKE_MK = 1

################################################################################
# PC virtual board makefile
################################################################################

VAL_HW_STACK += pc

PATH_PC = ${PATH_SRCS}/hal/board/pc
PATHS_SOURCE_SEARCH += ${PATH_PC}
FILES_EXTRA_SRC += \
	${PATH_PC}/pc_console.c \
	${PATH_PC}/pc_gpio.c

CFLAGS += -DBOARD_pc

.include "${PATH_SRCS}/hal/host/freebsd/freebsd_make.mk"

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
