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
FILE_TARGET = ${PATH_BUILDS_TARGET}/TaskMate
FILE_AUTOCODE_TARGET = ${PATH_BUILDS}/autoCode

# USB
PATH_USBKEY = /media/usbkey
FILE_USBDEV = /dev/da0s1

# Build info
BUILD_INFO = ${PATH_BUILDS}/last_build_info.txt
BUILD_CNT_FILE = ${PATH_BUILDS_TARGET}/build_counter
VAL_TM_VERSION_FILE = ${PATH_SOURCES}/interfaces/tm_version

FILE_CLOCRAW = ${PATH_BUILDS}/cloc_raw
FILE_CLOCDATA = ${PATH_BUILDS}/cloc_data
FILE_MEMRAW = ${PATH_BUILDS_TARGET}/mem_raw
FILE_MEMDATA = ${PATH_BUILDS_TARGET}/mem_data

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
FILE_AUTOCODE_CONFIG = ${PATH_BUILDS_TARGET}/autoCode_config

# git ignore
FILE_GIT_IGNORE = .gitignore
FILE_GIT_IGNORE_STAMP = ${PATH_BUILDS_TARGET}/.gitignore_stamp
PATHS_GIT_ALLOWED_PATH = ${PATH_DOCS} ${PATH_MAKEFILES} ${PATH_SOURCES} ${PATH_BUILDS} ${SCRIPTS_DIR}
VAL_GIT_ALLOWED.${PATH_DOCS} = *.c *.md *.txt *.png *.jpg Doxyfile
VAL_GIT_ALLOWED.${PATH_MAKEFILES} = *.mk *.conf
VAL_GIT_ALLOWED.${PATH_SOURCES} = *.c *.h *.rc *.err *.mk tm_version
VAL_GIT_ALLOWED.${PATH_BUILDS} = build_counter
VAL_GIT_ALLOWED.${SCRIPTS_DIR} = *.awk
VAL_GIT_ALLOWED = .clang-format .clang-tidy \
	audit_todo CHANGELOG LICENSE Makefile README.md

# Global error
FILE_ERROR_CAT = ${PATH_BUILDS_TARGET}/errors_all.err

# Tags
FILE_TAGS = .tags
FILE_TAGS_STAMP = ${PATH_BUILDS_TARGET}/.tags_stamp

# USB key directory backup
VAL_TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${VAL_TM_VERSION}
