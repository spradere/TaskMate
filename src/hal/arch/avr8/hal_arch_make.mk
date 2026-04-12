################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

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
CFLAGS += -I${SRC_DIR}
CFLAGS += -DTM_VERSION=\"${TM_VERSION}\" -DBUILD_CNT=${BUILD_CNT} \
	-DARCH_${ARCH} -DMCU_${MCU} -DBOARD_${BOARD}

# Linker flags
CFLAGS += -ffunction-sections -fdata-sections -flto
LFLGAS = -Wl,--gc-sections -Wl,-Map=${TARGET}.map

# output files
HEX = ${TARGET}.hex
ELF = ${TARGET}.elf

# link
${TARGET}: ${OBJS}
	@printf "\n%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${CC} ${CFLAGS} ${LFLAGS} -o ${ELF} ${OBJS}
	@printf "\t *.o -> ${ELF}\n"

# compile
${OBJS}: ${.TARGET:${BUILD_DIR_TARGET}%.o=${SRC_DIR}%.c}
	@printf "\n%sCompilation ...%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s> -> <%s>\n" ${.TARGET:${BUILD_DIR_TARGET}%.o=${SRC_DIR}%.c} ${.TARGET}
	@mkdir -p ${.TARGET:H}
	@${CC} ${CFLAGS} ${CFLAGS_${.TARGET:${BUILD_DIR_TARGET}%.o=${SRC_DIR}%.c}} \
		-c ${.TARGET:${BUILD_DIR_TARGET}%.o=${SRC_DIR}%.c} -o ${.TARGET}

upload: all _mcu_memory
#@ [avr8] Upload firmware to mcu via Arduino board.
	@printf "\n%sUpload binary to AVR flash, build %i %s\n\n" \
		"${COLOUR_TARGET_INFO}" ${BUILD_CNT} "${COLOUR_RESET}"
	# ELF to hex format
	@avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	# Upload to Atmega
	avrdude -c ${PROGRAMMER} -p ${MCU} -U flash:w:${HEX}:i -P ${PORT} -D
.PHONY: upload

# memory usage
_mcu_memory:
	@printf "\nStatic RAM usage : \n"
	@avr-size -G -d ${BUILD_DIR_TARGET}/TaskMate.elf
	@printf "\n"
	@avr-size -G -d ${BUILD_DIR_TARGET}/TaskMate.elf | awk -v flash_total="${FLASH_SIZE}" -v ram_total="${RAM_SIZE}" '\
	NR==2 { \
		text  = $$1; \
		data  = $$2; \
		bss   = $$3; \
		flash = text + data; \
		ram   = data + bss; \
		flash_pct = (flash / flash_total) * 100; \
		ram_pct   = (ram / ram_total) * 100; \
		printf("${COLOUR_WHITE_BOLD}Memory usage\n"); \
		printf("\tFlash : %d / %d bytes (%.1f%%)\n", flash, flash_total, flash_pct); \
		printf("\tRAM   : %d / %d bytes (%.1f%%)\n", ram, ram_total, ram_pct); \
		printf("${COLOUR_RESET}"); \
	}'

dump: all
#@ [avr8] Disassemble machine code in .hex and .elf
	@printf "\n%sGenerate debugging informations%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	avr-objcopy -O ihex -R .eeprom ${ELF} ${HEX}
	avr-objdump -D -m avr6 ${HEX} > "${BUILD_DIR_TARGET}/hex.txt"
	avr-objdump -D -m avr6 ${ELF} > "${BUILD_DIR_TARGET}/elf.txt"
.PHONY: dump

tidy_TaskMate:
#@ [avr8] tidy static code analysis for TaskMate, configuration /.clang-tidy.
	@printf "\n%sTidy TaskMate static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
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
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	avr-size -G -d ${BUILD_DIR_TARGET}/TaskMate.elf
	avr-nm --format=bsd --size-sort -r ${BUILD_DIR_TARGET}/TaskMate.elf | head -20

.PHONY: mem_size
