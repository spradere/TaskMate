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

# Default target
ARCH ?= avr8
VAL_MCU_SERIAL ?= atmega2560
BOARD ?= arduinoMega

# Valid arch
VALID_ARCHS = avr8 amd64 arm32v7-m4

.if empty(VALID_ARCHS:M${ARCH})
.error Invalid ARCH="${ARCH}". Valid values: ${VALID_ARCHS}
.endif

# Valid VAL_MCU_SERIAL for .arch
VALID_VAL_MCU_SERIALS.avr8 = atmega2560
VALID_VAL_MCU_SERIALS.amd64 = amd64
VALID_VAL_MCU_SERIALS.arm32v7-m4 = stm32g474

.if empty(VALID_VAL_MCU_SERIALS.${ARCH}:M${VAL_MCU_SERIAL})
.error Invalid VAL_MCU_SERIAL="${VAL_MCU_SERIAL}" for ARCH="${ARCH}". Valid values: ${VALID_VAL_MCU_SERIALS.${ARCH}}
.endif

# Valid boards for .mcu
VALID_BOARD.atmega2560 = arduinoMega arduinoMega_old
VALID_BOARD.stm32g474 = nucleo-g474
VALID_BOARD.amd64 = pc

.if empty(VALID_BOARD.${VAL_MCU_SERIAL}:M${BOARD})
.error Invalid BOARD="${BOARD}" for VAL_MCU_SERIAL="${VAL_MCU_SERIAL}". Valid boards: ${VALID_BOARD.${VAL_MCU_SERIAL}}
.endif
