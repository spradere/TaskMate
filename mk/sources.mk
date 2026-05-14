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

# Sources files and objects
FILES_SRC != find ${PATHS_SOURCES} ${OPT_FIND_EXCLUDE} -type f -name "*.c"
FILES_SRC_H != find ${PATHS_SOURCES} ${OPT_FIND_EXCLUDE} -type f -name "*.h"

FILES_OBJ = ${FILES_SRC:${PATH_SOURCES}/%.c=${PATH_BUILD_TARGET}/%.o}

# Dependency files
FILES_DEP = ${FILES_OBJ:.o=.d}
FILE_DEPS_ALL = ${PATH_BUILD_TARGET}/.deps.d

.sinclude "${FILE_DEPS_ALL}"

# autoCode
FILES_AUTOCOE_SRC != find ${PATH_SOURCES}/autoCode ${OPT_FIND_EXCLUDE} -type f -name "*.c"
FILES_AUTOCOE_SRC_H != find ${PATH_SOURCES}/autoCode ${OPT_FIND_EXCLUDE} -type f -name "*.h"
FILE_AUTOCODE_STAMP = ${PATH_BUILD_TARGET}/.autoCode_stamp

VAL_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"
FILE_AUTOCODE_LOG_STAMP = ${FILE_AUTOCODE_LOG}_${VAL_DATE_TIME}

FILES_INITRC != find ${PATHS_SOURCES} ${OPT_FIND_EXCLUDE} -type f -name "*.rc"

# Global error
FILES_ERROR != find ${PATHS_SOURCES} ${OPT_FIND_EXCLUDE}  -type f -name "*.err" | sort

# Documentation files
FILES_DOC != find ${PATH_DOCS} ${OPT_FIND_EXCLUDE} -type f -name "*.md"; \
		find ${PATH_DOCS} ${OPT_FIND_EXCLUDE} -type f -name "*.txt"

# mk files
FILES_MK_MK != find  ./${PATH_MAKEFILES} ${OPT_FIND_EXCLUDE} -type f -name "*.mk"
FILES_MK_HAL != find ./${PATH_SOURCES}/hal ${OPT_FIND_EXCLUDE} -type f -name "*.mk"

FILES_MK = ./Makefile ${FILES_MK_MK} ${FILES_MK_HAL}
