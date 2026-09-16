################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_MCU_ATMEGA2560_AT2560_MAKE_MK
HAL_MCU_ATMEGA2560_AT2560_MAKE_MK = 1

################################################################################
# ATMega 2560 makefile
################################################################################

# Registration in the global system
VAL_HW_STACK += atmega2560

PATH_ATMEGA2560 = ${PATH_SRCS}/hal/mcu/atmega2560
PATHS_SOURCE_SEARCH += ${PATH_ATMEGA2560}
FILES_EXTRA_SRC += ${PATH_ATMEGA2560}/at2560_gpio.c
FILES_EXTRA_SRC += ${PATH_ATMEGA2560}/at2560_init.c
FILES_HALINIT_HEADER += ${PATH_ATMEGA2560}/at2560_init.h
FILES_HALDEFINE += ${PATH_ATMEGA2560}/at2560_define.h
VAL_FUNCINIT := hal_atmega2560Init ${VAL_FUNCINIT}

CFLAGS += -DMCU_atmega2560

.include "${PATH_SRCS}/hal/arch/avr8/avr8_make.mk"

# MCU settings
VAL_MCU_SERIAL = atmega2560
VAL_MCU_ARCHTYPE = avr6

# ATmega2560 flash is 128 kB; PCHH is not implemented in the context switch,
# so flash is limited to 64 kB.
VAL_FLASH_SIZE_K=64
VAL_RAM_SIZE_K=8

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
