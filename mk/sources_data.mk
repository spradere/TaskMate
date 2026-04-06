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
SRCS != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.c"
SRCS_H != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.h"

OBJS = ${SRCS:${SRC_DIR}/%.c=${BUILD_DIR_TARGET}/%.o}

# Dependency files
DEPS = ${OBJS:.o=.d}
DEPS_FILE = ${BUILD_DIR_TARGET}/.deps.d

.sinclude "${DEPS_FILE}"

# autoCode
AUTOCODE_SRCS != find ${SRC_DIR}/autoCode -maxdepth 1 -type f -name "*.c"
AUTOCODE_SRCS_H != find ${SRC_DIR}/autoCode -maxdepth 1 -type f -name "*.h"
AUTOCODE_STAMP = ${BUILD_DIR_TARGET}/.autoCode_stamp

STAMP_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"
AUTOCODE_LOG_STAMP = ${AUTOCODE_LOG}_${STAMP_DATE_TIME}

FILES_INIT_RC != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.rc"

# Global error
ERROR_FILES != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.err" | sort

# Documentation files
DOCS != find  ${DOC_DIR} -maxdepth 1 -type f -name "*.md"; \
		find ${DOC_DIR} -maxdepth 1 -type f -name "*.txt"

# mk files
MK_FILES_MK != find  ./${MAKE_DIR} -maxdepth 1 -type f -name "*.mk"
MK_FILES_HAL != find ./${SRC_DIR}/hal -maxdepth 3 -type f -name "*.mk"

MK_FILES = ./Makefile ${MK_FILES_MK} ${MK_FILES_HAL}

################################################################################
# Data
################################################################################

# Build counter
.if !exists(${BUILD_CNT_FILE})
BUILD_CNT_BASE = 0
.else
BUILD_CNT_BASE != cat "${BUILD_CNT_FILE}"
.endif
BUILD_CNT = ${BUILD_CNT_BASE}

.if make(upload)
BUILD_CNT != NEW_BUILD=$$((${BUILD_CNT_BASE} + 1)); \
	printf "%i" $$NEW_BUILD > "${BUILD_CNT_FILE}"; \
	printf "%i" $$NEW_BUILD;
.endif

# Get upstream
UPSTREAM != git rev-parse --abbrev-ref --symbolic-full-name @{u}
