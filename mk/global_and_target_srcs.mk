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
SRC_DIR_LIST = ${SRC_DIR}/services/
SRC_DIR_LIST += ${SRC_DIR}/sysCore/
SRC_DIR_LIST += ${SRC_DIR}/tasks/
SRC_DIR_LIST += ${SRC_DIR}/libc/

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
DEPS_FILE = .deps.d

.sinclude "${DEPS_FILE}"

# autoCode
AUTOCODE_TARGET = utility/autoCode
AUTOCODE_SRC != find utility/autoCode_src/ -maxdepth 1 -type f -name "*.c"

# Initrc files for autocode
FILES_INIT_RC = ${SRC_DIR}/hal/arch/${ARCH}/drivers_init.rc
FILES_INIT_RC += ${SRC_DIR}/hal/mcu/${MCU}/drivers_init.rc
FILES_INIT_RC += ${SRC_DIR}/hal/board/${BOARD}/drivers_init.rc

FILES_INIT_RC += ${SRC_DIR}/services/services_init.rc
FILES_INIT_RC += ${SRC_DIR}/tasks/tasks_init.rc
