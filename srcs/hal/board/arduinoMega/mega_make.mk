################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_BOARD_ARDUINOMEGA_MEGA_MAKE_MK
HAL_BOARD_ARDUINOMEGA_MEGA_MAKE_MK = 1

################################################################################
# Arduino Mega board makefile
################################################################################

# Registration in the global system
VAL_HW_STACK += arduinoMega
PATH_ARDUINOMEGA = ${PATH_SRCS}/hal/board/arduinoMega
PATHS_SOURCE_SEARCH += ${PATH_ARDUINOMEGA}

CFLAGS += -DBOARD_arduinoMega

.include "${PATH_SRCS}/hal/mcu/atmega2560/at2560_make.mk"

# Board settings
VAL_CPU_FREQ = 16000000UL

VAL_PROGRAMMER = avrispmkII
.if ${OPT_ENVIRONMENT} == "freebsd"
VAL_PROGRAMMER_PORT = /dev/ttyU0
.endif

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
