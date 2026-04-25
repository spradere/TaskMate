################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# VAL_MCU_SERIAL settings

VAL_MCU_SERIAL = atmega2560
CFLAGS += -mmcu=${VAL_MCU_SERIAL}

# atmega2560 flash is 128 kB, PCHH is not implemented in context switch so flash is limited to 64 kB
VAL_FLASH_SIZE_K=64
VAL_RAM_SIZE_K=8
