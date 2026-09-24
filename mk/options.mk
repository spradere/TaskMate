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
# Build options
################################################################################

# General options
.MAIN: all
.NOTPARALLEL:

# autoCode
VAL_BUILD_AUTOCODE_EXPECTED_VER_MAJOR=1
VAL_BUILD_AUTOCODE_EXPECTED_VER_MINOR=2

OPT_CLEAN_AUTOCODE_LOGS = yes

.if ${OPT_CLEAN_AUTOCODE_LOGS} != "yes" && ${OPT_CLEAN_AUTOCODE_LOGS} != "no"
.error Invalid option OPT_CLEAN_AUTOCODE_LOGS : "${OPT_CLEAN_AUTOCODE_LOGS}". Valid values: [ yes | no ]
.endif

# Verbose level
OPT_VERBOSE_LEVEL ?= 0

.if !empty(VARIABLE:M[0-9]*)
.error Invalid option for OPT_VERBOSE_LEVEL : "${OPT_VERBOSE_LEVEL}", should be a number.
.endif

.if (${OPT_VERBOSE_LEVEL} < 0) || (${OPT_VERBOSE_LEVEL} > 2)
.error Invalid option for OPT_VERBOSE_LEVEL : "${OPT_VERBOSE_LEVEL}", should be [ 0 | 1 ].
.endif

_QUIET = ${${OPT_VERBOSE_LEVEL} == 1 :? : @}

# bmake option for -V (Win10/Cygwin)
.MAKE.EXPAND_VARIABLES = true


