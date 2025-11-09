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

# Compiler
CC = avr-gcc

CFLAGS += -Os -Wall -MMD -MP
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
