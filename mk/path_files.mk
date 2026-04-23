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
# Directories, path and files
################################################################################

# Targets
TARGET = ${PATH_BUILD_TARGET}/TaskMate
AUTOCODE_TARGET = ${PATH_BUILD}/autoCode

# USB
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1

# Build info
BUILD_INFO = ${PATH_BUILD}/last_build_info.txt
BUILD_CNT_FILE = ${PATH_BUILD_TARGET}/build_counter
VAL_TM_VERSION_FILE = ${PATH_SOURCES}/interfaces/tm_version

CLOC_RAW = ${PATH_BUILD_TARGET}/cloc_raw
CLOC_DATA = ${PATH_BUILD_TARGET}/cloc_data
MEM_RAW = ${PATH_BUILD_TARGET}/mem_raw
MEM_DATA = ${PATH_BUILD_TARGET}/mem_data

# Source directories
PATH_SOURCES_LIST = ${PATH_SOURCES}
PATH_SOURCES_LIST += ${PATH_SOURCES}/sysCore
PATH_SOURCES_LIST += ${PATH_SOURCES}/sysCall
PATH_SOURCES_LIST += ${PATH_SOURCES}/services
PATH_SOURCES_LIST += ${PATH_SOURCES}/tasks
PATH_SOURCES_LIST += ${PATH_SOURCES}/tm_libc
PATH_SOURCES_LIST += ${PATH_SOURCES}/interfaces

PATH_SOURCES_LIST += ${PATH_SOURCES}/hal
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/arch/${ARCH}
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/mcu/${MCU}
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/board/${BOARD}
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/public

# log files
AUTOCODE_LOG = ${PATH_LOGS}/autoCode_log
RSYNC_LOG = ${PATH_LOGS}/rsync.log
H_CHECK_LOG = ${PATH_LOGS}/headers_check.log

# scripts
SCRIPTS_DIR = scripts
H_ALLOW_FILE = mk/header_allow.conf

# autoCode
AUTOCODE_CONFIG = ${PATH_BUILD_TARGET}/autoCode_config

# git ignore
GIT_IGNORE = .gitignore
GIT_IGNORE_STAMP = ${PATH_BUILD_TARGET}/.gitignore_stamp
GIT_ALLOWED_DIRS = ${PATH_DOCS} ${PATH_MAKEFILES} ${PATH_SOURCES} ${PATH_BUILD} ${SCRIPTS_DIR}
GIT_ALLOWED_FILES.${PATH_DOCS} = *.c *.md *.txt *.png *.jpg Doxyfile
GIT_ALLOWED_FILES.${PATH_MAKEFILES} = *.mk *.conf
GIT_ALLOWED_FILES.${PATH_SOURCES} = *.c *.h *.rc *.err *.mk tm_version
GIT_ALLOWED_FILES.${PATH_BUILD} = build_counter
GIT_ALLOWED_FILES.${SCRIPTS_DIR} = *.awk
GIT_ALLOWED_FILES = .clang-format .clang-tidy \
	audit_todo CHANGELOG LICENSE Makefile README.md

# Global error
ERROR_CAT = ${PATH_BUILD_TARGET}/errors_all.err

# Tags
TAGS = .tags
TAGS_STAMP = ${PATH_BUILD_TARGET}/.tags_stamp

# USB key directory backup
TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${VAL_TM_VERSION}
