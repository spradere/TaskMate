################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# Registration in the global system
VAL_HW_STACK += atmega2560
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/mcu/atmega2560
CFLAGS += -DHW_MCU_atmega2560

.include "${PATH_SOURCES}/hal/arch/avr8/hal_arch_make.mk"

# MCU settings
VAL_MCU_SERIAL = atmega2560
CFLAGS += -mmcu=${VAL_MCU_SERIAL}

# atmega2560 flash is 128 kB, PCHH is not implemented in context switch so flash is limited to 64 kB
VAL_FLASH_SIZE_K=64
VAL_RAM_SIZE_K=8
