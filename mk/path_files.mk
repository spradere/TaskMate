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
PATH_BUILD_TARGET = ${PATH_BUILDS}/${VAL_HW_STACK:ts_}

# Targets
FILE_TARGET = ${PATH_BUILD_TARGET}/TaskMate
FILE_AUTOCODE_TARGET = ${PATH_BUILDS}/autoCode
FILE_PROGRAMS_CHECK_STAMP = ${PATH_BUILDS}/.programs_check_stamp

# Programs called by Makefiles
VAL_REQUIRED_PROGRAMS = awk \
	avr-gcc \
	avr-nm \
	avr-objcopy \
	avr-objdump \
	avr-size \
	avrdude \
	bmake \
	cat \
	clang \
	clang-format19 \
	clang-tidy19 \
	cloc \
	cppcheck \
	ctags \
	cut \
	date \
	doxygen \
	find \
	geany \
	git \
	grep \
	head \
	ls \
	mkdir \
	mount \
	printf \
	rm \
	rsync \
	sed \
	sort \
	touch \
	umount \
	vim \
	xargs

# USB
PATH_USBKEY = /media/usbkey
FILE_USBDEV = /dev/da0s1

# Build info
FILE_BUILD_INFO = ${PATH_BUILDS}/last_build_info.txt

FILE_CLOCRAW = ${PATH_BUILDS}/cloc_raw
FILE_CLOCDATA = ${PATH_BUILDS}/cloc_data
FILE_MEMRAW = ${PATH_BUILD_TARGET}/mem_raw
FILE_MEMDATA = ${PATH_BUILD_TARGET}/mem_data

FILE_TASKMATE_INFO = ${PATH_SRCS}/interfaces/taskmate_info.h

# Source directories
PATHS_SOURCES += ${PATH_SRCS}/system
PATHS_SOURCES += ${PATH_SRCS}/tm_libc
PATHS_SOURCES += ${PATH_SRCS}/interfaces
PATHS_SOURCES += ${PATH_SRCS}/hal/public
PATHS_SOURCES += ${PATH_SRCS}/hal/drivers
PATHS_SOURCES += ${PATH_SRCS}/user/tasks

# log files
FILE_AUTOCODE_LOG = ${PATH_LOGS}/autoCode_log
FILE_RSYNC_LOG = ${PATH_LOGS}/rsync.log
FILE_H_CHECK_LOG = ${PATH_LOGS}/headers_check.log

# scripts
FILE_H_ALLOW_CONF = mk/header_allow.conf

# autoCode
FILE_AUTOCODE_CONFIG = ${PATH_BUILD_TARGET}/autoCode_config
FILE_INITRC_LIST = ${PATH_BUILD_TARGET}/files_initrc
FILE_AUTOCODE_STAMP = ${PATH_BUILD_TARGET}/.autoCode_stamp
FILE_AUTOCODE_LOG_STAMP = ${FILE_AUTOCODE_LOG}_${VAL_DATE_TIME}

FILES_PARSE_TAG =  \
	${PATH_SRCS}/system/sysCall/error.c \
	${PATH_SRCS}/interfaces/error_catalog.h \
	${PATH_SRCS}/interfaces/modules_define.h \
	${PATH_SRCS}/system/sysCore/modules.c \
	${PATH_SRCS}/system/sysCore/modules_list.h \
	${PATH_SRCS}/system/sysCore/hal_init.h \
	${PATH_SRCS}/hal/public/define.h \
	${PATH_SRCS}/interfaces/gpio_signals.h 	
		
FILE_PARSE_TAG_LIST = ${PATH_BUILD_TARGET}/files_to_parse
FILE_HALINIT_LIST = ${PATH_BUILD_TARGET}/files_halinit
FILE_HALDEFINE_LIST = ${PATH_BUILD_TARGET}/files_haldefine

# git ignore
FILE_GIT_IGNORE = .gitignore
PATHS_GIT_ALLOWED = ${PATH_DOCS} ${PATH_MAKEFILES} ${PATH_SCRIPTS} ${PATH_SRCS}
FILES_GIT_ALLOWED.${PATH_DOCS} = *.c *.md *.txt *.png *.jpg Doxyfile
FILES_GIT_ALLOWED.${PATH_MAKEFILES} = *.mk *.conf
FILES_GIT_ALLOWED.${PATH_SRCS} = *.c *.h *.rc *.err *.mk *.gpio
FILES_GIT_ALLOWED.${PATH_SCRIPTS} = *.awk
FILES_GIT_ALLOWED = .clang-format .clang-tidy AGENTS.md\
	audit_todo CHANGELOG LICENSE Makefile README.md
FILES_GIT_EXCLUDED = \
	${PATH_SRCS}/interfaces/taskmate_info.h

# Global error
FILE_ERROR_LIST = ${PATH_BUILD_TARGET}/errors_all.err

# Tags
FILE_TAGS = .tags
FILE_TAGS_STAMP = ${PATH_BUILD_TARGET}/.tags_stamp

# USB key directory backup
VAL_TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${VAL_TM_VERSION}
