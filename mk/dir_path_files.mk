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
# Directories, path and files list
################################################################################

# Targets
TARGET = ${BUILD_DIR}/TaskMate
AUTOCODE_TARGET = ${BUILD_DIR}/autoCode

# List for hal headers
FILE_HAL_USER_PATH = ${BUILD_DIR}/files_hal_user
FILE_HAL_SYSTEM_PATH = ${BUILD_DIR}/files_hal_system

# log files
AUTOCODE_LOG = ${LOG_DIR}/autoCode_log
RSYNC_LOG = ${LOG_DIR}/rsync.log

# autoCode
AUTOCODE_CONFIG = ${BUILD_DIR}/autoCode_config

# Global error
ERROR_CAT = ${BUILD_DIR}/errors_all.err
ERROR_FILES != find ${SRC_DIR_LIST} -name '*.err' -maxdepth 1 -type f | sort

# git ignore
GIT_IGNORE = .gitignore
GIT_ALLOWED_DIR = ${DOC_DIR} ${MAKE_DIR} ${SRC_DIR}
GIT_ALLOWED_EXT.${DOC_DIR} = .c .md .txt .png .jpg
GIT_ALLOWED_EXT.${MAKE_DIR} = .mk
GIT_ALLOWED_EXT.${SRC_DIR} = .c .h .rc .err
GIT_ALLOWED_FILES = .clang-format .clang-tidy .gitingnore \
	audit_todo CHANGELOG LICENSE Makefile README.md version.h

# documentation files
DOCS != find ${DOC_DIR} -maxdepth 1 -type f -name "*.md"; find ${DOC_DIR} -maxdepth 1 -type f -name "*.txt"

# mk files
MK_FILES_MK != find ./${MAKE_DIR} -maxdepth 1 -type f -name "*.mk"
MK_FILES_HAL != find ./${SRC_DIR}/hal -maxdepth 3 -type f -name "*.mk"

MK_FILES = ./Makefile ${MK_FILES_MK} ${MK_FILES_HAL}

