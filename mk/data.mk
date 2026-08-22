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
VAL_TM_VERSION != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || printf "0.00"
VAL_TM_VER_MAJOR != echo ${VAL_TM_VERSION} | cut -d'.' -f1
VAL_TM_VER_MINOR != echo ${VAL_TM_VERSION} | cut -d'.' -f2

# Build counter
VAL_BUILD_CNT != git rev-list --count HEAD

# Get upstream
VAL_UPSTREAM != git rev-parse --abbrev-ref --symbolic-full-name @{u}
