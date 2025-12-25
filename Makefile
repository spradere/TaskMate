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

# Target choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduino_mega
.include "mk/target.mk"

# Make global process
.include "mk/colors.mk"
.include "mk/global_and_target_srcs.mk"
.include "mk/allow.mk"
.include "mk/global_build.mk"
.include "mk/global_utils.mk"
.include "mk/global_backup.mk"

# Target specific makefiles

.include "src/hal/arch/${ARCH}/make_arch.mk"
.include "src/hal/mcu/${MCU}/make_mcu.mk"
.include "src/hal/board/${BOARD}/make_board.mk"

