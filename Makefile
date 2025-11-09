################################################################################
#
# TaskMate Project
# (c) 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2025 PRADERE Sebastien
#
################################################################################

# Arch/mcu/board choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduino_mega
# valid archs
VALID_ARCHS = avr8 amd64 arm32v7-m4

ARCH ?= avr8
MCU ?= atmega2560
BOARD ?= arduino_mega

.if empty(VALID_ARCHS:M${ARCH})
.error Invalid ARCH="${ARCH}". Valid values: ${VALID_ARCHS}
.endif

# valid MCU for arch
VALID_MCUS.avr8 = atmega2560
VALID_MCUS.amd64 = amd64
VALID_MCUS.arm32v7-m4 = stm32g474

.if empty(VALID_MCUS.${ARCH}:M${MCU})
.error Invalid MCU="${MCU}" for ARCH="${ARCH}". Valid values: ${VALID_MCUS}.${ARCH}
.endif

# valid boards for mcu
VALID_BOARD.atmega2560 = arduino_mega
VALID_BOARD.stm32g474 = nucleo-g474
VALID_BOARD.amd64 = pc

.if empty(VALID_BOARD.${MCU}:M${BOARD})
.error Invalid BOARD="${BOARD}" for MCU="${MCU}". Valid boards: ${VALID_BOARD.${MCU}}
.endif

.info Building for architecture: ${ARCH}/${MCU}/${BOARD}


# Make process

.include "mk/global_and_arch_srcs.mk"

.include "mk/global_build.mk"

.include "mk/global_utils.mk"

.include "src/arch/${ARCH}/make_arch.mk"
.include "src/arch/${ARCH}/${MCU}/make_mcu.mk"
.include "src/arch/${ARCH}/${MCU}/${BOARD}/make_board.mk"

.include "mk/global_backup.mk"
