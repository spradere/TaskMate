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
# Build options
################################################################################

# autoCode
OPT_CLEAN_AUTOCODE_LOGS = yes
VALID_OPTS = yes no

.if empty(VALID_OPTS:M${OPT_CLEAN_AUTOCODE_LOGS})
.error Invalid option "${OPT_CLEAN_AUTOCODE_LOGS}". Valid values: [${VALID_OPTS}]
.endif

# bmake option for -V
.MAKE.EXPAND_VARIABLES = true

