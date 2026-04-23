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

# Sources files and objects
FILES_SRC != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.c"
FILES_SRC_H != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.h"

FILES_OBJ = ${FILES_SRC:${PATH_SOURCES}/%.c=${PATH_BUILD_TARGET}/%.o}

# Dependency files
FILES_DEP = ${FILES_OBJ:.o=.d}
FILE_DEPS_ALL = ${PATH_BUILD_TARGET}/.deps.d

.sinclude "${FILE_DEPS_ALL}"

# autoCode
FILES_AUTOCOE_SRC != find ${PATH_SOURCES}/autoCode -maxdepth 1 -type f -name "*.c"
FILES_AUTOCOE_SRC_H != find ${PATH_SOURCES}/autoCode -maxdepth 1 -type f -name "*.h"
FILE_AUTOCODE_STAMP = ${PATH_BUILD_TARGET}/.autoCode_stamp

VAL_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"
FILE_AUTOCODE_LOG_STAMP = ${FILE_AUTOCODE_LOG}_${VAL_DATE_TIME}

FILES_INIT_RC != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.rc"

# Global error
FILES_ERROR != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.err" | sort

# Documentation files
FILES_DOC != find ${PATH_FILES_DOC} -maxdepth 3 -type f -name "*.md"; \
		find ${PATH_FILES_DOC} -maxdepth 3 -type f -name "*.txt"

# mk files
FILES_MK_MK != find  ./${PATH_MAKEFILES} -maxdepth 1 -type f -name "*.mk"
FILES_MK_HAL != find ./${PATH_SOURCES}/hal -maxdepth 3 -type f -name "*.mk"

FILES_MK = ./Makefile ${FILES_MK_MK} ${FILES_MK_HAL}

################################################################################
# Data
################################################################################

# Build counter
#BUILD_REV != git rev-list --count HEAD
#BUILD_HASH != git rev-parse --short HEAD

.if make(upload)
VAL_BUILD_CNT != awk '{print $$1 + 1}' "${VAL_BUILD_CNT_FILE}"
.else
VAL_BUILD_CNT != awk '{print $$1;}' "${VAL_BUILD_CNT_FILE}"
.endif

# Get upstream
VAL_UPSTREAM != git rev-parse --abbrev-ref --symbolic-full-name @{u}
