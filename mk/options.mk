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

# Environment 
HOST ?= freebsd
OPT_ENVIRONMENT := ${HOST}

.if ${OPT_ENVIRONMENT} != "freebsd" && ${OPT_ENVIRONMENT} != "linux" && ${OPT_ENVIRONMENT} != "w10-cygwin"
.error Invalid option OPT_ENVIRONMENT : "${OPT_ENVIRONMENT}". Valid values: [ freebsd | linux | w10-cygwin ]
.endif


# Global find option
OPT_FIND_EXCLUDE = ! -path '*/.*'

# autoCode
VAL_BUILD_AUTOCODE_EXPECTED_VER_MAJOR=1
VAL_BUILD_AUTOCODE_EXPECTED_VER_MINOR=2

OPT_CLEAN_AUTOCODE_LOGS = yes

.if ${OPT_CLEAN_AUTOCODE_LOGS} != "yes" && ${OPT_CLEAN_AUTOCODE_LOGS} != "no"
.error Invalid option OPT_CLEAN_AUTOCODE_LOGS : "${OPT_CLEAN_AUTOCODE_LOGS}". Valid values: [ yes | no ]
.endif

# Verbose level
# Short name for command line
VERBOSE ?=0
OPT_VERBOSE_LEVEL = ${VERBOSE}

.if empty(OPT_VERBOSE_LEVEL:M[0-9]*)
.error Invalid option for OPT_VERBOSE_LEVEL : "${OPT_VERBOSE_LEVEL}", should be a number.
.endif

.if (${OPT_VERBOSE_LEVEL} < 0) || (${OPT_VERBOSE_LEVEL} > 2)
.error Invalid option for OPT_VERBOSE_LEVEL : "${OPT_VERBOSE_LEVEL}", should be [ 0 | 1 ].
.endif

_QUIET = ${${OPT_VERBOSE_LEVEL} == 1 :? : @}

.if ${OPT_ENVIRONMENT} == "w10-cygwin"
# bmake option for -V 
.MAKE.EXPAND_VARIABLES = true
.endif

