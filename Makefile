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


# Arch choice : make ARCH=avr8  (défautl avr8)
VALID_ARCHS = avr8 amd64 arm32v7-m4
ARCH ?= avr8

.if empty(VALID_ARCHS:M${ARCH})
.error Invalid ARCH="${ARCH}". Valid values: ${VALID_ARCHS}
.endif
.info Building for architecture: ${ARCH}

# Make process


.include "mk/global_and_arch_srcs.mk"

.include "mk/global_build.mk"

.include "mk/global_utils.mk"

.include "mk/${ARCH}_vars.mk"
.include "mk/${ARCH}_utils.mk"

.include "mk/global_backup.mk"
