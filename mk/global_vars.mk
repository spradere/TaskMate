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
# Variables
################################################################################

# Source directory
SRC_DIR = src
SRC_DIR_LIST = src/drivers/
SRC_DIR_LIST += src/services/
SRC_DIR_LIST += src/sysCore/
SRC_DIR_LIST += src/tasks/
SRC_DIR_LIST += src/libc/

# Automatically gather all sources files
SRCS != find ${SRC_DIR_LIST} -name "*.c"
SRCS_H != find ${SRC_DIR_LIST} -name "*.h"

# Build files and directory
BUILD_DIR = build
TARGET = ${BUILD_DIR}/TaskMate

OBJS = ${SRCS:${SRC_DIR}/%.c=${BUILD_DIR}/%.o}

# Dependency files
DEPS = ${OBJS:.o=.d}
DEPS_FILE = .deps.d

# autoCode
AUTOCODE_TARGET = utility/autoCode
AUTOCODE_SRC != find utility/autoCode_src/ -name "*.c"

# Initrc files for autocode
FILES_INIT_RC = src/drivers/drivers_init.rc
FILES_INIT_RC += src/services/services_init.rc
FILES_INIT_RC += src/tasks/tasks_init.rc

# Get git tag for USB key directory backup
USB_DIR = /media/usbkey
USB_DEV = /dev/da0s1
GIT_TAG != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || echo "0.00"
TASKMATE_DIR != printf "/code/TaskMate/TaskMate_%s" ${GIT_TAG}
