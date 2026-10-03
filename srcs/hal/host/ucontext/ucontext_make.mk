################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HOST_UCONTEXT_UCONTEXT_MAKE_MK
HOST_UCONTEXT_UCONTEXT_MAKE_MK = 1

################################################################################
# ucontext host makefile
################################################################################
VAL_HW_STACK += ucontext

PATH_UCONTEXT = ${PATH_SRCS}/hal/host/ucontext
PATHS_SOURCE_SEARCH += ${PATH_UCONTEXT}
FILES_EXTRA_SRC += \
	${PATH_UCONTEXT}/ucontext_context.c

FILE_HAL_STRING_MACRO = ${PATH_UCONTEXT}/ucontext_string_macro.h
FILE_HAL_ARCH_TYPES = ${PATH_UCONTEXT}/ucontext_types.h

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
