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

# Project metadata
.include "${CONF_TASKMATE_VERSION}"
.include "${CONF_BUILD_COUNTER}"

VAL_TM_VER_MAJOR != echo ${VAL_TM_VERSION} | cut -d'.' -f1
VAL_TM_VER_MINOR != echo ${VAL_TM_VERSION} | cut -d'.' -f2

# USB key directory backup
VAL_TM_BACKUP_DIR != printf "/code/TaskMate/TaskMate_%s" ${VAL_TM_VERSION}
