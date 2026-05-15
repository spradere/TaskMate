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
PATHS_SOURCES += ${PATH_SRCS_SYSTEM}/sysCore
PATHS_SOURCES += ${PATH_SRCS_SYSTEM}/sysCall
PATHS_SOURCES += ${PATH_SRCS_SYSTEM}/services
PATHS_SOURCES += ${PATH_SRCS_SYSTEM}/tm_libc
PATHS_SOURCES += ${PATH_SRCS_SYSTEM}/interfaces
PATHS_SOURCES += ${PATH_SRCS_HAL}/public
PATHS_SOURCES += ${PATH_SRCS_USER}/tasks

# log files
FILE_AUTOCODE_LOG = ${PATH_LOGS}/autoCode_log
FILE_RSYNC_LOG = ${PATH_LOGS}/rsync.log
FILE_H_CHECK_LOG = ${PATH_LOGS}/headers_check.log

# scripts
PATH_SCRIPTS = scripts
FILE_H_ALLOW_CONF = mk/header_allow.conf

# autoCode
FILE_AUTOCODE_CONFIG = ${PATH_BUILD_TARGET}/autoCode_config
FILE_INITRC_LIST = ${PATH_BUILD_TARGET}/files_initrc

FILES_PARSE_TAG = ${PATH_SRCS_SYSTEM}/sysCore/runLevel.h \
	${PATH_SRCS_SYSTEM}/sysCore/runLevel.c \
	${PATH_SRCS_SYSTEM}/sysCall/error.c \
	${PATH_SRCS_SYSTEM}/interfaces/error_catalog.h \
	${PATH_SRCS_SYSTEM}/interfaces/modules_define.h \
	${PATH_SRCS_SYSTEM}/sysCore/modules.c \
	${PATH_SRCS_SYSTEM}/sysCore/modules_list.h \
	${PATH_SRCS_SYSTEM}/sysCore/hal_init.h \
	${PATH_SRCS_HAL}/public/hal_define.h \
	${PATH_SRCS_HAL}/public/hal_sysInfo.c 	
	
FILE_PARSE_TAG_LIST = ${PATH_BUILD_TARGET}/files_to_parse
FILE_HALINIT_LIST = ${PATH_BUILD_TARGET}/files_halinit
FILE_HALDEFINE_LIST = ${PATH_BUILD_TARGET}/files_haldefine

# git ignore
FILE_GIT_IGNORE = .gitignore
FILE_GIT_IGNORE_STAMP = ${PATH_BUILD_TARGET}/.gitignore_stamp
PATHS_GIT_ALLOWED = ${PATH_DOCS} ${PATH_MAKEFILES} ${PATH_SRCS_SYSTEM} ${PATH_SCRIPTS}
VAL_GIT_ALLOWED.${PATH_DOCS} = *.c *.md *.txt *.png *.jpg Doxyfile
VAL_GIT_ALLOWED.${PATH_MAKEFILES} = *.mk *.conf
VAL_GIT_ALLOWED.${PATH_SRCS_SYSTEM} = *.c *.h *.rc *.err *.mk
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
