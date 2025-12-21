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

# Compiler to add : -Wredundant-decls
CC = avr-gcc

CFLAGS += -Os -Wall -Wextra -Wshadow -Wstrict-prototypes -Wconversion \
	-Wcast-align -Wundef -Wnull-dereference -Wpointer-arith -Wcast-qual \
	-Wmissing-prototypes -Wmissing-declarations -Wredundant-decls \
	-Wundef -Wswitch-enum -Wformat=2 -Wformat-security -Wpointer-arith \
	-MMD -MP -Wno-builtin-declaration-mismatch -Wno-return-type
CFLAGS += -I/root/code/TaskMate/TaskMate_current/src

# Files
HEX = ${TARGET}.hex
ELF = ${TARGET}.elf

# Flash Gordon
upload:all
	@printf "\n\033[1;33mUpload binary to AVR flash\033[0m\n\n"
	# ELF to hex format
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	# RAM usage
	@printf "\nStatic RAM usage : "
	avr-size -B ${ELF}
	@printf "\n"
	# Upload to Atmega
	avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${HEX}:i -P ${PORT} -D
.PHONY: upload

# Disassemble machine code in two formats
dump:all
	@printf "\n\033[1;33mGenerate debugging informations\033[0m\n\n"
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > ${BUILD_DIR}/hex.txt
	avr-objdump -D -m avr6 ${ELF} > ${BUILD_DIR}/elf.txt
.PHONY: dump

# clang-tidy
TIDY_SRC = ${AUTOCODE_SRC}
TIDY_SRC += ${SRCS}

tidy:
	@printf "\n\033[1;33mTidy static test code, config in src/.clang-tidy\033[0m\n\n"
	@clang-tidy $(TIDY_SRC) -- \
		-I/root/code/TaskMate/TaskMate_current/src \
		-I/root/code/TaskMate/TaskMate_current/ \
		-isystem /usr/local/avr/include -isystem /usr/local/lib/gcc/avr/14.1.0 \
		-D__AVR__=6 -D__AVR_ATmega2560__=1 \
		-DF_CPU=${F_CPU} \
		-DHAL_SYSTEM_CRITICAL_API_ALLOWED \
		-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED

.PHONY: tidy
