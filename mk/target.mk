################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2026 PRADERE Sebastien
#
################################################################################

################################################################################
# Target selection and test
################################################################################

# Default target
ARCH ?= avr8
MCU ?= atmega2560
BOARD ?= arduino_mega

# Valid arch
VALID_ARCHS = avr8 amd64 arm32v7-m4

.if empty(VALID_ARCHS:M${ARCH})
.error Invalid ARCH="${ARCH}". Valid values: ${VALID_ARCHS}
.endif

# Valid MCU for .arch
VALID_MCUS.avr8 = atmega2560
VALID_MCUS.amd64 = amd64
VALID_MCUS.arm32v7-m4 = stm32g474

.if empty(VALID_MCUS.${ARCH}:M${MCU})
.error Invalid MCU="${MCU}" for ARCH="${ARCH}". Valid values: ${VALID_MCUS.${ARCH}}
.endif

# Valid boards for .mcu
VALID_BOARD.atmega2560 = arduino_mega arduino_mega_old
VALID_BOARD.stm32g474 = nucleo-g474
VALID_BOARD.amd64 = pc

.if empty(VALID_BOARD.${MCU}:M${BOARD})
.error Invalid BOARD="${BOARD}" for MCU="${MCU}". Valid boards: ${VALID_BOARD.${MCU}}
.endif

# Final target
.info Building for architecture: ${ARCH} -> ${MCU} -> ${BOARD}

