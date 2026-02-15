################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the TaskMate License v1.0.
# See the LICENSE file for full license terms.
#
# Non-commercial use permitted under conditions. Commercial use requires a separate license.
# Commercial licensing inquiries: https://codeberg.org/Doul09/TaskMate/issues
#
# Powered by TaskMate, (c) 2026 PRADERE Sebastien
#
################################################################################

# compiler for arch avr8
CC = avr-gcc

# General options
CFLAGS += -Os -MMD -MP

# General warnings
CFLAGS += -Wall -Wextra -Wshadow -Werror=shadow -Wswitch -Wswitch-enum -Wformat=2 -Wformat-security

# Prototypes
CFLAGS += -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls \
	-Wbad-function-cast

# Conversions
CFLAGS += -Wconversion -Wsign-conversion -Wenum-conversion -Wpointer-arith -Wcast-align -Wcast-qual

# Bug hunter
CFLAGS += -Wnull-dereference -Wundef -Werror=undef -Werror=implicit-function-declaration \
	-Werror=return-type -Wdouble-promotion -Wwrite-strings -fno-common

# Commande line #include and #define
CFLAGS += -I${SRC_DIR} -DARCH=\"${ARCH}\" -DMCU=\"${MCU}\" -DBOARD=\"${BOARD}\"

# Linker flags
CFLAGS += -ffunction-sections -fdata-sections -flto -fstack-usage
LFLGAS = -Wl,--gc-sections -Wl,-Map=${TARGET}.map

# output files
HEX = ${TARGET}.hex
ELF = ${TARGET}.elf

# link
${TARGET}: ${OBJS}
	@printf "\n%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	${CC} ${CFLAGS} ${LFLAGS} -o ${ELF} ${OBJS}

# compile
${OBJS}: ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c}
	@printf "\n%sCompilation ...%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s> -> <%s>\n" ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} ${.TARGET}
	@mkdir -p ${.TARGET:H}
	@${CC} ${CFLAGS} ${CFLAGS_${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c}} \
		-c ${.TARGET:${BUILD_DIR}%.o=${SRC_DIR}%.c} -o ${.TARGET}

upload: all
#@ [avr8] Upload firmware to mcu via Arduino board.
	@printf "\n%sUpload binary to AVR flash%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	# ELF to hex format
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	# RAM usage
	@printf "\nStatic RAM usage : "
	avr-size -G -d ${BUILD_DIR}/TaskMate.elf
	@printf "\n"
	# Upload to Atmega
	avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${HEX}:i -P ${PORT} -D
.PHONY: upload

dump: all
#@ [avr8] Disassemble machine code in .hex and .elf
	@printf "\n%sGenerate debugging informations%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > ${BUILD_DIR}/hex.txt
	avr-objdump -D -m avr6 ${ELF} > ${BUILD_DIR}/elf.txt
.PHONY: dump

tidy_TaskMate:
#@ [avr8] tidy static code analysis for TaskMate, configuration /.clang-tidy.
	@printf "\n%sTidy TaskMate static code test%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	@clang-tidy $(SRCS) ${SRCS_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/${SRC_DIR}/ \
	-isystem /usr/local/avr/include \
	-isystem /usr/local/lib/gcc/avr/14.1.0 \
	-D__AVR__=6 -D__AVR_ATmega2560__=1 \
	-DF_CPU=${F_CPU} \
	-DHAL_SYSTEM_CRITICAL_API_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED
.PHONY: tidy_TaskMate

mem_size: all
#@ [avr8] List module size sorted from highest.
	@printf "\n%sList module size%s\n\n" \
		"${COLOR_TARGET_INFO}" "${COLOR_RESET}"
	avr-size -G -d ${BUILD_DIR}/TaskMate.elf
	avr-nm --format=bsd --size-sort -r ${BUILD_DIR}/TaskMate.elf | head -20

.PHONY: module_size
