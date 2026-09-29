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
# test1 hardware target
################################################################################

#help TARGET=test1: [test1] Build the test1 target for Arduino Mega.

# Registration in the global system
VAL_HW_STACK = test1

PATH_TEST1 = ${PATH_SRCS}/user/target/test1
PATHS_SOURCE_SEARCH += ${PATH_TEST1}
PATHS_EXTRA_SRC += 	${PATH_SRCS}/tmLibc

FILE_GPIO_SIGNALS = ${PATH_TEST1}/signals.gpio
FILE_WIREGPIO = ${PATH_TEST1}/targetWireSignal.c

CFLAGS += -DHWT_test1
.include "${PATH_SRCS}/hal/board/arduinoMega/mega_make.mk"

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
