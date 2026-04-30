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
VAL_HW_STACK += avr8
PATH_SOURCES_LIST += ${PATH_SOURCES}/hal/arch/avr8
CFLAGS += -DHW_ARCH_avr8
# Compilation redirection
FILE_ARCH_CC = ${PATH_SOURCES}/hal/arch/avr8/hal_archCC.mk

# compiler for arch avr8
CC = avr-gcc
CC_VER != avr-gcc -dumpversion

# General options
CFLAGS += -Os -MMD -MP

# General warnings
CFLAGS += -Wall -Wextra -Wshadow -Werror=shadow -Wswitch -Wswitch-enum -Wformat=2 -Wformat-security

# Prototypes
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls \
	-Wbad-function-cast

# Conversions
CFLAGS += -Wconversion -Wsign-conversion -Wenum-conversion -Wcast-align -Wcast-qual

# Bug hunter
CFLAGS += -Wnull-dereference -Wundef -Werror=undef -Werror=implicit-function-declaration \
	-Werror=return-type -Wdouble-promotion -Wwrite-strings -fno-common -Wpointer-arith

# Command line #include and #define
CFLAGS += -I${PATH_SOURCES}
CFLAGS += -DVAL_TM_VERSION=\"${VAL_TM_VERSION}\" -DVAL_BUILD_CNT=${VAL_BUILD_CNT}

# Linker flags
CFLAGS += -ffunction-sections -fdata-sections -flto
LFLGAS = -Wl,--gc-sections -Wl,-Map=${FILE_TARGET}.map




