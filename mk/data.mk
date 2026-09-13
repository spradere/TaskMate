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
# Data
################################################################################

# TaskMate version from tag
VAL_TM_VERSION != ${SCRIPT_GIT_VERSION}
VAL_TM_VER_MAJOR != echo ${VAL_TM_VERSION} | cut -d'.' -f1
VAL_TM_VER_MINOR != echo ${VAL_TM_VERSION} | cut -d'.' -f2

# USB key directory backup
VAL_TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${VAL_TM_VERSION}

# Build counter
VAL_BUILD_CNT != git rev-list --count HEAD
