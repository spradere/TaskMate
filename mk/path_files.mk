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

# Final full HW path
#PATH_BUILD_TARGET = ${PATH_BUILDS}/${ARCH}_${MCU}_${BOARD}
PATH_BUILD_TARGET = ${PATH_BUILDS}/${VAL_HW_STACK:ts_}

# Targets
FILE_TARGET = ${PATH_BUILDS_TARGET}/TaskMate
FILE_AUTOCODE_TARGET = ${PATH_BUILDS}/autoCode

# USB
PATH_USBKEY = /media/usbkey
FILE_USBDEV = /dev/da0s1

# Build info
FILE_BUILD_INFO = ${PATH_BUILDS}/last_build_info.txt
FILE_TM_INFO = ${PATH_BUILDS}/tm_info.txt

FILE_CLOCRAW = ${PATH_BUILDS}/cloc_raw
FILE_CLOCDATA = ${PATH_BUILDS}/cloc_data
FILE_MEMRAW = ${PATH_BUILD_TARGET}/mem_raw
FILE_MEMDATA = ${PATH_BUILD_TARGET}/mem_data

# Source directories
PATH_SOURCES_LIST += ${PATH_SOURCES}/sysCore
PATH_SOURCES_LIST += ${PATH_SOURCES}/sysCall
PATH_SOURCES_LIST += ${PATH_SOURCES}/services
PATH_SOURCES_LIST += ${PATH_SOURCES}/tasks
PATH_SOURCES_LIST += ${PATH_SOURCES}/tm_libc
PATH_SOURCES_LIST += ${PATH_SOURCES}/interfaces
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/public

# log files
FILE_AUTOCODE_LOG = ${PATH_LOGS}/autoCode_log
FILE_RSYNC_LOG = ${PATH_LOGS}/rsync.log
FILE_H_CHECK_LOG = ${PATH_LOGS}/headers_check.log

# scripts
PATH_SCRIPTS = scripts
FILE_H_ALLOW_CONF = mk/header_allow.conf

# autoCode
FILE_AUTOCODE_CONFIG = ${PATH_BUILD_TARGET}/autoCode_config
FILE_INITRC_LIST = ${PATH_BUILD_TARGET}/files_initrc_list

FILES_PARSE_TAG = src/sysCore/runLevel.h \
	src/sysCore/runLevel.c \
	src/sysCall/error.c \
	src/interfaces/error_catalog.h \
	src/hal/public/hal_sysInfo.c \
	src/interfaces/modules_define.h \
	src/sysCore/modules.c \
	src/sysCore/modules_list.h \
	src/hal/public/hal_define.h \
	src/sysCore/hal_init.h

FILE_PARSE_TAG_LIST = ${PATH_BUILD_TARGET}/files_to_parse_list

# git ignore
FILE_GIT_IGNORE = .gitignore
FILE_GIT_IGNORE_STAMP = ${PATH_BUILD_TARGET}/.gitignore_stamp
PATHS_GIT_ALLOWED_PATH = ${PATH_DOCS} ${PATH_MAKEFILES} ${PATH_SOURCES} ${PATH_SCRIPTS}
VAL_GIT_ALLOWED.${PATH_DOCS} = *.c *.md *.txt *.png *.jpg Doxyfile
VAL_GIT_ALLOWED.${PATH_MAKEFILES} = *.mk *.conf
VAL_GIT_ALLOWED.${PATH_SOURCES} = *.c *.h *.rc *.err *.mk
VAL_GIT_ALLOWED.${PATH_SCRIPTS} = *.awk
VAL_GIT_ALLOWED = .clang-format .clang-tidy \
	audit_todo CHANGELOG LICENSE Makefile README.md

# Global error
FILE_ERROR_LIST = ${PATH_BUILD_TARGET}/errors_all.err

# Tags
FILE_TAGS = .tags
FILE_TAGS_STAMP = ${PATH_BUILD_TARGET}/.tags_stamp

# USB key directory backup
VAL_TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${VAL_TM_VERSION}
