################################################################################
#
# TaskMate Project
# (c) 2025 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2025 PRADERE Sebastien
#
################################################################################

################################################################################
# Sources
################################################################################

# Source directory
SRC_DIR = src
SRC_DIR_LIST = ${SRC_DIR}/sysCore/
SRC_DIR_LIST += ${SRC_DIR}/sysCall/
SRC_DIR_LIST += ${SRC_DIR}/services/
SRC_DIR_LIST += ${SRC_DIR}/tasks/
SRC_DIR_LIST += ${SRC_DIR}/libc/

SRC_DIR_LIST += ${SRC_DIR}/hal/
SRC_DIR_LIST += ${SRC_DIR}/hal/arch/${ARCH}/
SRC_DIR_LIST += ${SRC_DIR}/hal/mcu/${MCU}/
SRC_DIR_LIST += ${SRC_DIR}/hal/board/${BOARD}/

# Automatically gather all sources files
SRCS != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.c"
SRCS_H != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.h"

# Build files and directory
BUILD_DIR = build
TARGET = ${BUILD_DIR}/TaskMate

OBJS = ${SRCS:${SRC_DIR}/%.c=${BUILD_DIR}/%.o}

# Dependency files
DEPS = ${OBJS:.o=.d}
DEPS_FILE = build/.deps.d

.sinclude "${DEPS_FILE}"

# autoCode
AUTOCODE_TARGET = utility/autoCode
AUTOCODE_SRCS != find utility/autoCode_src/ -maxdepth 1 -type f -name "*.c"
AUTOCODE_SRCS_H != find utility/autoCode_src/ -maxdepth 1 -type f -name "*.h"
AUTOCODE_STAMP = build/.autoCode_stamp_${ARCH}_${MCU}_${BOARD}
AUTOCODE_DATE_TIME != date +"%Y_%m_%d_%H:%M:%S"

# Initrc files for autocode
FILES_INIT_RC != find ${SRC_DIR_LIST} -maxdepth 1 -type f -name "*.rc"

# glabal errors
ERROR_ALL = build/errors_all.err
ERROR_FILES != find ${SRC_DIR_LIST} -name '*.err' -maxdepth 1 -type f | sort

# mk file -> target help
MK_FILES_MK != find ./mk -maxdepth 1 -type f -name "*.mk"
MK_FILES_HAL != find ./src/hal -maxdepth 3 -type f -name "*.mk"

MK_FILES = ./Makefile ${MK_FILES_MK} ${MK_FILES_HAL}
