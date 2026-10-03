################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

.ifndef HAL_ARCH_AVR8_AVR8_MAKE_MK
HAL_ARCH_AVR8_AVR8_MAKE_MK = 1

################################################################################
# AVR architecture makefile
################################################################################

# Registration in the global system
VAL_HW_STACK += avr8

PATH_AVR8 = ${PATH_SRCS}/hal/arch/avr8
PATHS_SOURCE_SEARCH += ${PATH_AVR8}
PATHS_EXTRA_SRC += ${PATH_SRCS}/hal/arch/avr8

FILE_HAL_STRING_MACRO = ${PATH_SRCS}/hal/arch/avr8/avr8_string_macro.h
FILE_HAL_ARCH_TYPES = ${PATH_AVR8}/avr8_types.h
	
# Compilation redirection
FILE_ARCH_CC = ${PATH_SRCS}/hal/arch/avr8/avr8_CC.mk

# Compiler for the AVR8 architecture
CC = avr-gcc
VAL_CC_VERSION != avr-gcc -dumpversion

# General options
CFLAGS += -Os -MMD -MP -mrelax -fshort-enums -mcall-prologues
CFLAGS += -DF_CPU=${VAL_CPU_FREQ} -mmcu=${VAL_MCU_SERIAL}

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
CFLAGS += -I${PATH_SRCS} -I.

# Linker flags
CFLAGS += -ffunction-sections -fdata-sections -flto
LDFLAGS = -Wl,--gc-sections -Wl,-Map=${FILE_TARGET}.map

# Linters
# Clang rejects the GCC naked context implementation even with the AVR target.
FILES_CLANG_TIDY_SRC = ${FILES_COMPILE_SRC:N${PATH_AVR8}/avr8_context.c}
CFLAGS_CLANG_TIDY = --target=avr -mmcu=${VAL_MCU_SERIAL} -std=gnu17
CFLAGS_CLANG_TIDY += ${CFLAGS:M-D*} ${CFLAGS:M-I*}
CFLAGS_CLANG_TIDY += -include ${FILE_HAL_ARCH_TYPES}
CFLAGS_CLANG_TIDY += -isystem /usr/local/avr/include

OPT_CPPCHECK_TARGET = --platform=avr8 --library=avr
# Cppcheck models AVR widths and library calls, but not the PROGMEM declaration macro.
CFLAGS_CPPCHECK = ${CFLAGS:M-D*} ${CFLAGS:M-I*} -DPROGMEM=
CFLAGS_CPPCHECK += --include=${FILE_HAL_ARCH_TYPES}
CFLAGS_CPPCHECK += --include=${FILE_HAL_STRING_MACRO}

.else
.error Multiple inclusion of ${.PARSEDIR}/${.PARSEFILE}
.endif
