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

# Source directories
SRC_DIR_LIST = ${SRC_DIR}/sysCore
SRC_DIR_LIST += ${SRC_DIR}/sysCall
SRC_DIR_LIST += ${SRC_DIR}/services
SRC_DIR_LIST += ${SRC_DIR}/tasks
SRC_DIR_LIST += ${SRC_DIR}/tm_libc

SRC_DIR_LIST += ${SRC_DIR}/hal
SRC_DIR_LIST += ${SRC_DIR}/hal/arch/${ARCH}
SRC_DIR_LIST += ${SRC_DIR}/hal/mcu/${MCU}
SRC_DIR_LIST += ${SRC_DIR}/hal/board/${BOARD}

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

FILES_HAL_USER != find ${SRC_DIR}/hal/ -name '*.h' -type f -exec grep -l '// @hal_user' {} +
FILES_HAL_SYSTEM != find ${SRC_DIR}/hal/ -name '*.h' -type f -exec grep -l '// @hal_system' {} +
FILE_HAL_USER_PATH = ${BUILD_DIR}/files_hal_user
FILE_HAL_SYSTEM_PATH = ${BUILD_DIR}/files_hal_system

AUTOCODE_CFLAGS = -I/root/code/TaskMate/TaskMate_current/${SRC_DIR}/
AUTOCODE_CFLAGS += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

FILES_INIT_RC != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.rc"

# Global erro
ERROR_FILES != find ${SRC_DIR_LIST} -name '*.err' -maxdepth 1 -type f | sort

# mk file -> target help
MK_FILES_MK != find ./${MK_DIR} -maxdepth 1 -type f -name "*.mk"
MK_FILES_HAL != find ./${SRC_DIR}/hal -maxdepth 3 -type f -name "*.mk"

MK_FILES = ./Makefile ${MK_FILES_MK} ${MK_FILES_HAL}
