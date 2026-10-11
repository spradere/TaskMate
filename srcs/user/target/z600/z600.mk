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
# z600 FreeBSD ucontext simulation target
################################################################################

#help TARGET=z600: [z600] Build the z600 FreeBSD ucontext simulation target.

VAL_HW_STACK = z600

PATH_TARGET_Z600 = ${PATH_SRCS}/user/target/z600
PATHS_SOURCE_SEARCH += ${PATH_TARGET_Z600}

.include "${PATH_SRCS}/hal/board/pc/pc_make.mk"

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
