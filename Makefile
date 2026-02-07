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

################################################################################
# Main makefile
################################################################################

# Main directories
SRC_DIR = src
BUILD_DIR = build
LOG_DIR = log
MAKE_DIR = mk
DOC_DIR = doc
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1

# Main path/file
FILE_HAL_USER_PATH = ${BUILD_DIR}/files_hal_user
FILE_HAL_SYSTEM_PATH = ${BUILD_DIR}/files_hal_system

AUTOCODE_LOG = ${LOG_DIR}/autoCode_log
RSYNC_LOG = ${LOG_DIR}/rsync.log

AUTOCODE_CONFIG = ${BUILD_DIR}/autoCode_config
ERROR_CAT = ${BUILD_DIR}/errors_all.err

GIT_IGNORE = .gitignore
GIT_ALLOWED_DIR = ${DOC_DIR} ${MAKE_DIR} ${SRC_DIR}
GIT_ALLOWED_EXT.${DOC_DIR} = .c .md .txt .png .jpg
GIT_ALLOWED_EXT.${MAKE_DIR} = .mk
GIT_ALLOWED_EXT.${SRC_DIR} = .c .h .rc .err
GIT_ALLOWED_FILES = .clang-format .clang-tidy .gitingnore \
	audit_todo CHANGELOG LICENSE Makefile README.md version.h

# Options
OPT_CLEAN_AUTOCODE_LOGS = yes

# Targets
TARGET = ${BUILD_DIR}/TaskMate
AUTOCODE_TARGET = ${BUILD_DIR}/autoCode

# Hardware target choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduino_mega
.include "${MAKE_DIR}/hardware_target.mk"

# Make global process
.include "${MAKE_DIR}/make_colours.mk"
.include "${MAKE_DIR}/global_and_target_srcs.mk"
.include "${MAKE_DIR}/header_allow.mk"
.include "${MAKE_DIR}/global_build.mk"
.include "${MAKE_DIR}/global_utils.mk"
.include "${MAKE_DIR}/global_backup.mk"
.include "${MAKE_DIR}/editors.mk"

# Hardware specific makefiles
.include "${SRC_DIR}/hal/arch/${ARCH}/arch_make.mk"
.include "${SRC_DIR}/hal/mcu/${MCU}/mcu_make.mk"
.include "${SRC_DIR}/hal/board/${BOARD}/board_make.mk"

