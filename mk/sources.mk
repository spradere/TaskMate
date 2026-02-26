################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2026 PRADERE Sebastien
#
################################################################################

################################################################################
# Sources
################################################################################

# Sources files and objects
SRCS != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.c"
SRCS_H != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.h"

OBJS = ${SRCS:${SRC_DIR}/%.c=${BUILD_DIR}/%.o}

# Dependency files
DEPS = ${OBJS:.o=.d}
DEPS_FILE = ${BUILD_DIR}/.deps.d

.sinclude "${DEPS_FILE}"

# autoCode
AUTOCODE_SRCS != find ${SRC_DIR}/autoCode -maxdepth 1 -type f -name "*.c"
AUTOCODE_SRCS_H != find ${SRC_DIR}/autoCode -maxdepth 1 -type f -name "*.h"
AUTOCODE_STAMP = ${BUILD_DIR}/.autoCode_stamp_${ARCH}_${MCU}_${BOARD}

STAMP_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"
AUTOCODE_LOG_STAMP = ${AUTOCODE_LOG}_${STAMP_DATE_TIME}

FILE_HAL_TMLIBC != find ${SRC_DIR}/hal -maxdepth 3 -type f -name "hal_tmlibc.h"

FIND_OPT_BASE = -maxdepth 1 -name '*.h' -type f -exec grep -l
FIND_OPT_USER_TAG = '// @hal_user'
FIND_OPT_SYSTEM_TAG = '// @hal_system'
CMD_OPT_OUT = {} + 2>/dev/null || true

FIND_OPT_USER = ${FIND_OPT_BASE} ${FIND_OPT_USER_TAG} ${CMD_OPT_OUT}
FIND_OPT_SYSTEM = ${FIND_OPT_BASE} ${FIND_OPT_SYSTEM_TAG} ${CMD_OPT_OUT}

FILES_HAL_PATH = 	${SRC_DIR}/hal/arch/${ARCH} \
					${SRC_DIR}/hal/mcu/${MCU} \
					${SRC_DIR}/hal/board/${BOARD}

FILES_HAL_USER != { \
    for d in ${FILES_HAL_PATH}; do \
        find "$$d" ${FIND_OPT_USER}; \
    done; }

FILES_HAL_SYSTEM != { \
    for d in ${FILES_HAL_PATH}; do \
        find "$$d" ${FIND_OPT_SYSTEM}; \
    done; }

FILES_INIT_RC != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.rc"

# Global error
ERROR_FILES != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.err" | sort

# Documentation files
DOCS != find  ${DOC_DIR} -maxdepth 1 -type f -name "*.md"; find ${DOC_DIR} -maxdepth 1 -type f -name "*.txt"

# mk files
MK_FILES_MK != find  ./${MAKE_DIR} -maxdepth 1 -type f -name "*.mk"
MK_FILES_HAL != find ./${SRC_DIR}/hal -maxdepth 3 -type f -name "*.mk"

MK_FILES = ./Makefile ${MK_FILES_MK} ${MK_FILES_HAL}

# TaskMate version
TM_VERSION != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || echo "0.00"

# Build counter
.if !exists(${BUILD_CNT_FILE})
BUILD_CNT_BASE = 0
.else
BUILD_CNT_BASE != cat ${BUILD_CNT_FILE}
.endif
BUILD_CNT = ${BUILD_CNT_BASE}

.if make(upload)
BUILD_CNT != NEW_BUILD=$$((${BUILD_CNT_BASE} + 1)); \
	echo $$NEW_BUILD > ${BUILD_CNT_FILE}; \
	echo $$NEW_BUILD;
.endif
