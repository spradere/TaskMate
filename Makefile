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
# Main makefile
################################################################################

# Main directories
SRC_DIR = src
BUILD_DIR = build
LOG_DIR = log
MAKE_DIR = mk
DOC_DIR = doc

# Build system options
.include "${MAKE_DIR}/options.mk"

# Hardware target choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduino_mega
.include "${MAKE_DIR}/hardware_target.mk"
BUILD_DIR_TARGET = ${BUILD_DIR}/${ARCH}_${MCU}_${BOARD}

# path and files
.include "${MAKE_DIR}/path_files.mk"

# Make global process
.include "${MAKE_DIR}/sources_data.mk"
.include "${MAKE_DIR}/header_allow.mk"
.include "${MAKE_DIR}/build.mk"
.include "${MAKE_DIR}/utils.mk"
.include "${MAKE_DIR}/backup.mk"
.include "${MAKE_DIR}/editors.mk"
.include "${MAKE_DIR}/colours.mk"

# Hardware specific makefiles
.include "${SRC_DIR}/hal/arch/${ARCH}/arch_make.mk"
.include "${SRC_DIR}/hal/mcu/${MCU}/mcu_make.mk"
.include "${SRC_DIR}/hal/board/${BOARD}/board_make.mk"

