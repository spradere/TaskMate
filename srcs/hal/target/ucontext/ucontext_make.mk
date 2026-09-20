################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_TARGET_UCONTEXT_UCONTEXT_MAKE_MK
HAL_TARGET_UCONTEXT_UCONTEXT_MAKE_MK = 1

################################################################################
# ucontext target makefile
################################################################################

PATH_UCONTEXT = ${PATH_SRCS}/hal/target/ucontext
PATHS_SOURCE_SEARCH += ${PATH_UCONTEXT}
FILES_EXTRA_SRC += \
	${PATH_UCONTEXT}/ucontext_context.c

CFLAGS += -DTARGET_ucontext
FILE_HAL_STRING_MACRO = ${PATH_UCONTEXT}/ucontext_string_macro.h
FILE_HAL_ARCHITECTURE_TYPES = ${PATH_UCONTEXT}/ucontext_types.h
CFLAGS += -include ${FILE_HAL_ARCHITECTURE_TYPES}

.include "${PATH_SRCS}/hal/board/pc/pc_make.mk"

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
