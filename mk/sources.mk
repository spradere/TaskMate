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
# Sources
################################################################################

OPT_FIND_EXCLUDE = ! -path '*/.*'

# Header files found across every selected source directory
FILES_SRC_H != find ${PATHS_SOURCE_SEARCH} ${OPT_FIND_EXCLUDE} -type f -name "*.h"
FILES_DRIVER_INTERFACES != find ${PATH_SRCS}/interfaces ${OPT_FIND_EXCLUDE} -type f \
	-name "drv_*.h" | sort

# init.rc source declarations
FILES_INITRC != find ${PATHS_SOURCE_SEARCH} ${OPT_FIND_EXCLUDE} -type f -name "*.rc"
FILES_INITRC_SRC != awk -v source_option="-source_file" -v source_root="${PATH_SRCS}" \
	-f "${SCRIPT_INITRC_SOURCES}" ${FILES_INITRC:M*_init.rc}
PATHS_INITRC_SOURCES != awk -v source_option="-source_dir" -v source_root="${PATH_SRCS}" \
	-f "${SCRIPT_INITRC_SOURCES}" ${FILES_INITRC:M*_init.rc}

FILES_INITRC_DIR_SRC =
.if !empty(PATHS_INITRC_SOURCES)
FILES_INITRC_DIR_SRC != find ${PATHS_INITRC_SOURCES} ${OPT_FIND_EXCLUDE} -type f -name "*.c"
.endif

# Base system sources compiled for every target
FILES_BASE_SYSTEM_SRC != find \
	${PATH_SRCS}/system/sysCore \
	${PATH_SRCS}/system/sysCall \
	${PATH_SRCS}/tmLibc \
	${OPT_FIND_EXCLUDE} -type f -name "*.c"
FILES_BASE_SYSTEM_SRC += ${PATH_SRCS}/system/TaskMate.c

# Complete, target-specific compilation list
FILES_COMPILE_SRC = \
	${FILES_BASE_SYSTEM_SRC} \
	${FILES_EXTRA_SRC} \
	${FILES_INITRC_SRC} \
	${FILES_INITRC_DIR_SRC}
FILES_COMPILE_SRC := ${FILES_COMPILE_SRC:O:u}

# Compatibility aliases preserve unchanged test contracts; build rules use the variables above.
FILES_SRC = ${FILES_COMPILE_SRC}
REFACTOR_FILES_SRC = ${FILES_INITRC_SRC}
REFACTOR_PATHS_SOURCES = ${FILES_INITRC_SRC:M*system/services/scli.c:S/scli.c/commands/}

FILES_OBJ = ${FILES_COMPILE_SRC:%.c=${PATH_BUILD_TARGET}/%.o}

# Dependency files
FILES_DEP = ${FILES_OBJ:.o=.d}
FILE_DEPS_ALL = ${PATH_BUILD_TARGET}/.deps.d

.sinclude "${FILE_DEPS_ALL}"

# autoCode
FILES_AUTOCODE_SRC != find ${PATH_SRCS}/autoCode ${OPT_FIND_EXCLUDE} -type f -name "*.c"
FILES_AUTOCODE_SRC_H != find ${PATH_SRCS}/autoCode ${OPT_FIND_EXCLUDE} -type f -name "*.h"

VAL_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"

# Global error
FILES_ERROR != find ${PATHS_SOURCE_SEARCH} ${OPT_FIND_EXCLUDE}  -type f -name "*.err" | sort
FILES_ERROR += ${PATH_SRCS}/hal/drivers_errors.err

# Documentation files
FILES_DOC != find ${PATH_DOCS} ${OPT_FIND_EXCLUDE} -type f -name "*.md"; \
		find ${PATH_DOCS} ${OPT_FIND_EXCLUDE} -type f -name "*.txt"

# Makefiles
FILES_MK_MK != find  ./${PATH_MAKEFILES} ${OPT_FIND_EXCLUDE} -type f -name "*.mk"
FILES_MK_HAL != find ./${PATH_SRCS}/hal ${OPT_FIND_EXCLUDE} -type f -name "*.mk"
FILES_MK_TEST != find ./${PATH_TEST} ${OPT_FIND_EXCLUDE} -type f -name "*.mk"

FILES_MK = ./Makefile ${FILES_MK_MK} ${FILES_MK_HAL} ${FILES_MK_TEST}
