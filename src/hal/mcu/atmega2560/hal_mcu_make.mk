################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# MCU settings

MCU = atmega2560
CFLAGS += -mmcu=${MCU}

# atmega2560 flash is 128 kB, PCHH is not implemented in context switch so flash is limited to 64 kB
FLASH_SIZE_K=64
RAM_SIZE_K=8
