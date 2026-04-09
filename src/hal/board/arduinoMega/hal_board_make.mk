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
F_CPU = 16000000UL

CFLAGS += -DF_CPU=${F_CPU}

PROGRAMMER = avrispmkII
PORT = /dev/ttyU0
