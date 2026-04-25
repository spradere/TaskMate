################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# board settings
VAL_CPU_FREQ = 16000000UL

CFLAGS += -DF_CPU=${VAL_CPU_FREQ}

VAL_PROGRAMMER = avrispmkII
VAL_PROGRAMMER_PORT = /dev/ttyU0
