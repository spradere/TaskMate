################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_MCU_MAKE_MK
HAL_MCU_MAKE_MK = 1

# Registration in the global system
VAL_HW_STACK += atmega2560
PATH_ATMEGA2560 = ${PATH_SOURCES}/hal/mcu/atmega2560
PATHS_SOURCES += ${PATH_ATMEGA2560}
FILES_HALINIT += ${PATH_ATMEGA2560}/hal_mcuInit.h
FILES_HALDEFINE += ${PATH_ATMEGA2560}/hal_mcu_define.h
CFLAGS += -DMCU_atmega2560

.include "${PATH_SOURCES}/hal/arch/avr8/hal_arch_make.mk"

# MCU settings
VAL_MCU_SERIAL = atmega2560
CFLAGS += -mmcu=${VAL_MCU_SERIAL}

# atmega2560 flash is 128 kB, PCHH is not implemented in context switch so flash is limited to 64 kB
VAL_FLASH_SIZE_K=64
VAL_RAM_SIZE_K=8

.endif
