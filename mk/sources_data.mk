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
SRCS != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.c"
SRCS_H != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.h"

OBJS = ${SRCS:${PATH_SOURCES}/%.c=${PATH_BUILDS_TARGET}/%.o}

# Dependency files
DEPS = ${OBJS:.o=.d}
DEPS_FILE = ${PATH_BUILDS_TARGET}/.deps.d

.sinclude "${DEPS_FILE}"

# autoCode
AUTOCODE_SRCS != find ${PATH_SOURCES}/autoCode -maxdepth 1 -type f -name "*.c"
AUTOCODE_SRCS_H != find ${PATH_SOURCES}/autoCode -maxdepth 1 -type f -name "*.h"
AUTOCODE_STAMP = ${PATH_BUILDS_TARGET}/.autoCode_stamp

STAMP_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"
AUTOCODE_LOG_STAMP = ${AUTOCODE_LOG}_${STAMP_DATE_TIME}

FILES_INIT_RC != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.rc"

# Global error
ERROR_FILES != find ${PATH_SOURCES_LIST} -maxdepth 1 -type f -name "*.err" | sort

# Documentation files
DOCS != find ${PATH_DOCS} -maxdepth 3 -type f -name "*.md"; \
		find ${PATH_DOCS} -maxdepth 3 -type f -name "*.txt"

# mk files
MK_FILES_MK != find  ./${PATH_MAKEFILES} -maxdepth 1 -type f -name "*.mk"
MK_FILES_HAL != find ./${PATH_SOURCES}/hal -maxdepth 3 -type f -name "*.mk"

MK_FILES = ./Makefile ${MK_FILES_MK} ${MK_FILES_HAL}

################################################################################
# Data
################################################################################

# Build counter
#BUILD_REV != git rev-list --count HEAD
#BUILD_HASH != git rev-parse --short HEAD

.if make(upload)
BUILD_CNT != awk '{print $$1 + 1}' "${BUILD_CNT_FILE}"
.else
BUILD_CNT != awk '{print $$1;}' "${BUILD_CNT_FILE}"
.endif

# Get upstream
UPSTREAM != git rev-parse --abbrev-ref --symbolic-full-name @{u}
