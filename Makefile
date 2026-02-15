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
BUILD_BASE = build
LOG_DIR = log
MAKE_DIR = mk
DOC_DIR = doc
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1

# Options
OPT_CLEAN_AUTOCODE_LOGS = yes
# bmake option for -V
.MAKE.EXPAND_VARIABLES = true

# Hardware target choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduino_mega
.include "${MAKE_DIR}/hardware_target.mk"
BUILD_DIR += ${BUILD_BASE}/${ARCH}_${MCU}_${BOARD}

# path and files
.include "${MAKE_DIR}/dir_path_files.mk"

# Make global process
.include "${MAKE_DIR}/sources.mk"
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

