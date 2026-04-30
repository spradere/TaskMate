################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# Registration in the global system
VAL_HW_STACK = test1
PATH_SOURCES_LIST = ${PATH_SOURCES}/hal/target/test1
CFLAGS += -DHWT_test1

.include "${PATH_SOURCES}/hal/board/arduinoMega/hal_board_make.mk"
