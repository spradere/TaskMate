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
# Main makefile
################################################################################

# Main directories
PATH_SOURCES = src
PATH_BUILDS = build
PATH_LOGS = log
PATH_MAKEFILES = mk
PATH_FILES_DOC = doc
PATH_SCRIPTS = scripts

# Build system options
.include "${PATH_MAKEFILES}/options.mk"

# Hardware target choice :  make ARCH=avr8 MCU=atmega2560 BOARD=arduinoMega
.include "${PATH_MAKEFILES}/hardware_target.mk"
PATH_BUILD_TARGET = ${PATH_BUILDS}/${ARCH}_${MCU}_${BOARD}

# Definitions
VAL_TM_VERSION != git describe --tags | cut -d'-' -f1 | sed 's/^v//' || printf "0.00"
.include "${PATH_MAKEFILES}/path_files.mk"
.include "${PATH_MAKEFILES}/sources_data.mk"
.include "${PATH_MAKEFILES}/header_allow.mk"
.include "${PATH_MAKEFILES}/colours.mk"

# Make global process
.include "${PATH_MAKEFILES}/build.mk"
.include "${PATH_MAKEFILES}/utils.mk"
.include "${PATH_MAKEFILES}/backup.mk"
.include "${PATH_MAKEFILES}/editors.mk"

# Make hardware specific
.include "${PATH_SOURCES}/hal/arch/${ARCH}/hal_arch_make.mk"
.include "${PATH_SOURCES}/hal/mcu/${MCU}/hal_mcu_make.mk"
.include "${PATH_SOURCES}/hal/board/${BOARD}/hal_board_make.mk"

