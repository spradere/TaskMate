################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_HOST_FREEBSD_FREEBSD_MAKE_MK
HAL_HOST_FREEBSD_FREEBSD_MAKE_MK = 1

################################################################################
# FreeBSD host makefile
################################################################################

VAL_HW_STACK += freebsd

PATH_FREEBSD = ${PATH_SRCS}/hal/host/freebsd
PATHS_SOURCE_SEARCH += ${PATH_FREEBSD}
FILES_EXTRA_SRC += \
	${PATH_FREEBSD}/freebsd_atomic.c \
	${PATH_FREEBSD}/freebsd_halt.c \
	${PATH_FREEBSD}/freebsd_interrupts.c \
	${PATH_FREEBSD}/freebsd_timerSched.c \
	${PATH_FREEBSD}/freebsd_timerSTC.c \
	${PATH_FREEBSD}/freebsd_usart.c

CFLAGS += -DHOST_freebsd
FILE_ARCH_CC = ${PATH_FREEBSD}/freebsd_CC.mk

CC = cc
VAL_CC_VERSION != ${CC} --version | head -n 1

CFLAGS += -std=gnu17 -O0 -g -MMD -MP
CFLAGS += -Wall -Wextra -Wshadow -Wswitch -Wswitch-enum -Wformat=2 -Wformat-security
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls
CFLAGS += -Wconversion -Wsign-conversion -Wenum-conversion -Wcast-align -Wcast-qual
CFLAGS += -Wnull-dereference -Wundef -Werror=undef -Werror=implicit-function-declaration
CFLAGS += -Werror=return-type -Wdouble-promotion -Wwrite-strings -fno-common -Wpointer-arith
CFLAGS += -I${PATH_SRCS} -I.
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS = -Wl,--gc-sections -lncursesw -ltinfow

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
