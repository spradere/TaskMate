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
	${PATH_FREEBSD}/freebsd_halt.cpp \
	${PATH_FREEBSD}/freebsd_interrupts.c

FILE_ARCH_CC = ${PATH_FREEBSD}/freebsd_CC.mk

CC = cc
CXX = c++
VAL_CC_VERSION != ${CC} --version | head -n 1

CFLAGS += -std=gnu17 -O0 -g -MMD -MP
CFLAGS += -Wall -Wextra -Wshadow -Wswitch -Wswitch-enum -Wformat=2 -Wformat-security
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls
CFLAGS += -Wconversion -Wsign-conversion -Wenum-conversion -Wcast-align -Wcast-qual
CFLAGS += -Wnull-dereference -Wundef -Werror=undef -Werror=implicit-function-declaration
CFLAGS += -Werror=return-type -Wdouble-promotion -Wwrite-strings -fno-common -Wpointer-arith
CFLAGS += -I${PATH_SRCS} -I.
CFLAGS += -DTM_LIBC_TASKMATE=0 -DTM_LIBC_CSTD=1
CFLAGS += -ffunction-sections -fdata-sections

CXXFLAGS = -std=c++20 -O0 -g -MMD -MP
CXXFLAGS += -Wall -Wextra -Wshadow -Wconversion -Wsign-conversion -Werror=return-type
CXXFLAGS += -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit
CXXFLAGS += -ffunction-sections -fdata-sections
CXXFLAGS += ${CFLAGS:M-I*} ${CFLAGS:M-D*}

LDFLAGS = -Wl,--gc-sections -Wl,-Map,${FILE_TARGET}.map -lncursesw -ltinfow

# Linters
FILE_CLANG_TIDY_TARGET != ${CC} -dumpmachine
CFLAGS_CLANG_TIDY = --target=${FILE_CLANG_TIDY_TARGET} -std=gnu17
CFLAGS_CLANG_TIDY += ${CFLAGS:M-D*} ${CFLAGS:M-I*}
CFLAGS_CLANG_TIDY += -include ${FILE_HAL_ARCH_TYPES}
CFLAGS_CLANG_TIDY_CPP = --target=${FILE_CLANG_TIDY_TARGET} -std=c++20
CFLAGS_CLANG_TIDY_CPP += ${CXXFLAGS:M-D*} ${CXXFLAGS:M-I*}
CFLAGS_CLANG_TIDY_CPP += -include ${FILE_HAL_ARCH_TYPES}
FILES_CLANG_TIDY_SRC = ${FILES_COMPILE_SRC}

OPT_CPPCHECK_TARGET = --platform=unix64 --library=bsd --library=posix
# ucontext is intentional, and cppcheck does not model hal_halt() as non-returning.
OPT_CPPCHECK_TARGET += --suppress=getcontextCalled --suppress=makecontextCalled
OPT_CPPCHECK_TARGET += --suppress=nullPointerRedundantCheck:${PATH_FREEBSD}/freebsd_timerSched.c
CFLAGS_CPPCHECK = ${CFLAGS:M-D*} ${CFLAGS:M-I*}
CFLAGS_CPPCHECK += --include=${FILE_HAL_ARCH_TYPES}
CFLAGS_CPPCHECK += --include=${FILE_HAL_STRING_MACRO}

.include "${PATH_SRCS}/hal/host/ucontext/ucontext_make.mk"

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
