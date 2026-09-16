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
# Directories, paths, and files
################################################################################

# Final full HW path
PATH_BUILD_TARGET = ${PATH_BUILDS}/${VAL_HW_STACK:ts_}
PATH_BUILD_GENERATED = ${PATH_BUILD_TARGET}/generated
PATH_LEGACY = legacy

# Targets
FILE_TARGET = ${PATH_BUILD_TARGET}/TaskMate
FILE_AUTOCODE_TARGET = ${PATH_BUILDS}/autoCode
FILE_PROGRAMS_CHECK_STAMP = ${PATH_BUILDS}/.programs_check_stamp

# Configuration
CONF_PROGRAMS_LIST = ${PATH_CONF}/programs-list.conf
CONF_HARDWARE_TARGETS = ${PATH_CONF}/hardware-targets.conf
CONF_SYSTEM_HEADER_ALLOW = ${PATH_CONF}/system_header_allow.conf

# Tests
PATH_AUTOCODE_TEST = ${PATH_TEST}/autoCode
PATH_BUILD_AUTOCODE_TEST = ${PATH_BUILDS}/autoCode_test
FILE_AUTOCODE_TEST_SANITIZE_TARGET = ${PATH_BUILDS}/autoCode_sanitize

PATH_BUILD_TEST = ${PATH_TEST}/build
PATH_BUILD_BUILD_TEST = ${PATH_BUILDS}/build_test

PATH_TM_STRING_TEST = ${PATH_TEST}/tm_string
FILE_TM_STRING_TEST = ${PATH_TM_STRING_TEST}/tm_string_test.c
FILE_TM_STRING_TEST_TARGET = ${PATH_BUILDS}/tm_string_test

# USB
PATH_USBKEY = /media/usbkey
FILE_USBDEV = /dev/da0s1

# Build information
FILE_BUILD_INFO = ${PATH_BUILDS}/last_build_info.txt

FILE_CLOCRAW = ${PATH_BUILDS}/cloc_raw
FILE_CLOCDATA = ${PATH_BUILDS}/cloc_data
FILE_MEMRAW = ${PATH_BUILD_TARGET}/mem_raw
FILE_MEMDATA = ${PATH_BUILD_TARGET}/mem_data
FILE_TM_INFO = ${PATH_SRCS}/interfaces/tm_info.h

# Source directories
PATHS_SOURCES += ${PATH_SRCS}/system
PATHS_SOURCES += ${PATH_SRCS}/tmLibc
PATHS_SOURCES += ${PATH_SRCS}/interfaces
PATHS_SOURCES += ${PATH_SRCS}/hal/public
PATHS_SOURCES += ${PATH_SRCS}/hal/drivers
PATHS_SOURCES += ${PATH_SRCS}/user/tasks

# Log files
FILE_AUTOCODE_LOG = ${PATH_LOGS}/autoCode_log
FILE_RSYNC_LOG = ${PATH_LOGS}/rsync.log
FILE_H_CHECK_LOG = ${PATH_LOGS}/headers_check.log
FILE_ARCH_CHECK_LOG = ${PATH_LOGS}/architecture_check.log

# Scripts
FILE_ARCH_VALID_MATRIX = ${PATH_CONF}/arch_valid_matrix.md
SCRIPT_ARCH_INCLUDE = ${PATH_SCRIPTS}/arch_include.awk
SCRIPT_AUTOCODE_LOG = ${PATH_SCRIPTS}/autocode_log.awk
SCRIPT_AUTOCODE_TEST = ${PATH_AUTOCODE_TEST}/autoCode_test.sh
SCRIPT_BUILD_SUMMARY_CLOC = ${PATH_SCRIPTS}/build_summary_cloc.awk
SCRIPT_BUILD_SUMMARY_MEMORY = ${PATH_SCRIPTS}/build_summary_memory.awk
SCRIPT_BUILD_TEST = ${PATH_BUILD_TEST}/build_test.sh
SCRIPT_CHECK_BUILD_DELETE_PATH = ${PATH_SCRIPTS}/check_build_delete_path.sh
SCRIPT_CHECK_PROGRAMS = ${PATH_SCRIPTS}/check_programs.sh
SCRIPT_CLOC_DATA = ${PATH_SCRIPTS}/cloc_data.awk
SCRIPT_CLOC_SHOW = ${PATH_SCRIPTS}/cloc_show.awk
SCRIPT_COMPARE_REPLACE = ${PATH_SCRIPTS}/compare_replace.sh
SCRIPT_GIT_VERSION = ${PATH_SCRIPTS}/git_version.sh
SCRIPT_HARDWARE_TARGET = ${PATH_SCRIPTS}/hardware_target.awk
SCRIPT_HEADER_ALLOW = ${PATH_SCRIPTS}/header_allow.awk
SCRIPT_MAKE_HELP = ${PATH_SCRIPTS}/make_help.awk

# Git
FILE_GIT_IGNORE = .gitignore
PATHS_GIT_ALLOWED = \
	${PATH_AGENTS} ${PATH_CONF} ${PATH_DOCS} ${PATH_MAKEFILES} ${PATH_SCRIPTS} \
	${PATH_SRCS} ${PATH_TEST} ${PATH_LEGACY}
FILES_GIT_ALLOWED.${PATH_AGENTS} = *.md *.yaml
FILES_GIT_ALLOWED.${PATH_CONF} = *.conf *.md
FILES_GIT_ALLOWED.${PATH_DOCS} = *.c *.md *.txt *.png *.jpg Doxyfile
FILES_GIT_ALLOWED.${PATH_MAKEFILES} = *.mk *
FILES_GIT_ALLOWED.${PATH_SRCS} = *.c *.h *.rc *.err *.mk *.gpio *.list *.awk
FILES_GIT_ALLOWED.${PATH_SCRIPTS} = *.awk *.sh
FILES_GIT_ALLOWED.${PATH_TEST} = *.c *.mk *.sh
FILES_GIT_ALLOWED.${PATH_LEGACY} = *.c *.h Makefile
FILES_GIT_ALLOWED = .clang-format .clang-tidy AGENTS.md\
	audit_todo CHANGELOG CHANGELOG_AUTOCODE_SYNTAX LICENSE Makefile README.md
FILES_GIT_EXCLUDED = \
	${FILE_TM_INFO}
	
# Tags
FILE_TAGS = .tags
FILE_TAGS_STAMP = ${PATH_BUILD_TARGET}/.tags_stamp
