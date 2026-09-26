################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef TARGET_MK
TARGET_MK = 1

################################################################################
# FreeBSD ucontext simulation target
################################################################################

VAL_HW_STACK = ucontext

PATH_TARGET_UCONTEXT = ${PATH_SRCS}/user/target/ucontext
PATHS_SOURCE_SEARCH += ${PATH_TARGET_UCONTEXT}
PATHS_EXTRA_SRC += ${PATH_SRCS}/tmLibc
FILES_EXTRA_SRC += \
	${PATH_SRCS}/system/services/commands/scli_driver.c \
	${PATH_SRCS}/system/services/commands/scli_stack.c \
	${PATH_SRCS}/system/services/commands/scli_thread.c

FILE_GPIO_SIGNALS = ${PATH_TARGET_UCONTEXT}/signals.gpio
FILE_WIREGPIO = ${PATH_TARGET_UCONTEXT}/targetWireSignal.c
FILE_WIREGPIO_TAG = ${PATH_SRCS}/hal/board/pc/pc_gpio.c

CFLAGS += -DHWT_ucontext
.include "${PATH_SRCS}/hal/target/ucontext/ucontext_make.mk"
CFLAGS_${PATH_TARGET_UCONTEXT}/ucontext_system.c += -include ${FILE_HAL_STRING_MACRO}

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
