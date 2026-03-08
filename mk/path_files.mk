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
# Directories, path and files
################################################################################

# Targets
TARGET = ${BUILD_DIR_TARGET}/TaskMate
AUTOCODE_TARGET = ${BUILD_DIR}/autoCode

# USB
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1

# Build info
BUILD_INFO = ${BUILD_DIR}/last_build_info.txt
BUILD_CNT_FILE = ${BUILD_DIR_TARGET}/build_counter
TM_VERSION_FILE = ${BUILD_DIR_TARGET}/tm_version

# Source directories
SRC_DIR_LIST = ${SRC_DIR}
SRC_DIR_LIST += ${SRC_DIR}/sysCore
SRC_DIR_LIST += ${SRC_DIR}/sysCall
SRC_DIR_LIST += ${SRC_DIR}/services
SRC_DIR_LIST += ${SRC_DIR}/tasks
SRC_DIR_LIST += ${SRC_DIR}/tm_libc
SRC_DIR_LIST += ${SRC_DIR}/interfaces

SRC_DIR_LIST += ${SRC_DIR}/hal
SRC_DIR_LIST += ${SRC_DIR}/hal/arch/${ARCH}
SRC_DIR_LIST += ${SRC_DIR}/hal/mcu/${MCU}
SRC_DIR_LIST += ${SRC_DIR}/hal/board/${BOARD}

# List for hal headers
FILE_HAL_USER_PATH = ${BUILD_DIR_TARGET}/files_hal_user
FILE_HAL_SYSTEM_PATH = ${BUILD_DIR_TARGET}/files_hal_system

# log files
AUTOCODE_LOG = ${LOG_DIR}/autoCode_log
RSYNC_LOG = ${LOG_DIR}/rsync.log

# autoCode
AUTOCODE_CONFIG = ${BUILD_DIR_TARGET}/autoCode_config

# git ignore
GIT_IGNORE = .gitignore
GIT_IGNORE_STAMP = ${BUILD_DIR_TARGET}/.gitignore_stamp
GIT_ALLOWED_DIRS = ${DOC_DIR} ${MAKE_DIR} ${SRC_DIR} ${BUILD_DIR}
GIT_ALLOWED_FILES.${DOC_DIR} = *.c *.md *.txt *.png *.jpg Doxyfile
GIT_ALLOWED_FILES.${MAKE_DIR} = *.mk
GIT_ALLOWED_FILES.${SRC_DIR} = *.c *.h *.rc *.err *.mk
GIT_ALLOWED_FILES.${BUILD_DIR} = build_counter
GIT_ALLOWED_FILES = .clang-format .clang-tidy \
	audit_todo CHANGELOG LICENSE Makefile README.md

# Global error
ERROR_CAT = ${BUILD_DIR_TARGET}/errors_all.err

# Tags
TAGS = .tags
TAGS_STAMP = ${BUILD_DIR_TARGET}/.tags_stamp

# USB key directory backup
TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${TM_VERSION}
