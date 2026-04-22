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
# Main makefile
################################################################################

# Main directories
SRC_DIR = src
BUILD_DIR = build
LOG_DIR = log
MAKE_DIR = mk
DOC_DIR = doc
DIR_SCRIPTS = scripts

# Build system options
.include "${MAKE_DIR}/options.mk"

# Hardware target choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduinoMega
.include "${MAKE_DIR}/hardware_target.mk"
BUILD_DIR_TARGET = ${BUILD_DIR}/${ARCH}_${MCU}_${BOARD}

# Definitions
TM_VERSION != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || printf "0.00"
.include "${MAKE_DIR}/path_files.mk"
.include "${MAKE_DIR}/sources_data.mk"
.include "${MAKE_DIR}/header_allow.mk"
.include "${MAKE_DIR}/colours.mk"

# Make global process
.include "${MAKE_DIR}/build.mk"
.include "${MAKE_DIR}/utils.mk"
.include "${MAKE_DIR}/backup.mk"
.include "${MAKE_DIR}/editors.mk"

# Make hardware specific
.include "${SRC_DIR}/hal/arch/${ARCH}/hal_arch_make.mk"
.include "${SRC_DIR}/hal/mcu/${MCU}/hal_mcu_make.mk"
.include "${SRC_DIR}/hal/board/${BOARD}/hal_board_make.mk"

