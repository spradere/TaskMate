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
# noscli hardware target
################################################################################

# Registration in the global system
VAL_HW_STACK = noscli

PATH_NOSCLI = ${PATH_SRCS}/user/target/noscli
PATHS_SOURCE_SEARCH += ${PATH_NOSCLI}
PATHS_EXTRA_SRC += 	${PATH_SRCS}/tmLibc

FILE_GPIO_SIGNALS = ${PATH_NOSCLI}/signals.gpio
FILE_WIREGPIO = ${PATH_NOSCLI}/targetWireSignal.c
FILE_WIREGPIO_TAG = ${PATH_SRCS}/hal/mcu/atmega2560/at2560_gpio.c

CFLAGS += -DHWT_noscli
.include "${PATH_SRCS}/hal/board/arduinoMega/mega_make.mk"

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
