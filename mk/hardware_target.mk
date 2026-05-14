################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

################################################################################
# Hardware target selection and test
################################################################################

# default target / new system
HWT ?= test1
#FILE_HWT_MK = ${PATH_SRCS_SYSTEM}/hal/target/${HWT}/hwt.mk
FILE_HWT_MK = ${PATH_SRCS_USER}/target/${HWT}/hwt.mk

.if !exists(${FILE_HWT_MK})
.error Target not found >>>${HWT}<<<
.endif

.include "${FILE_HWT_MK}"
