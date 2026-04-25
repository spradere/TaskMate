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
CFLAGS += -I${PATH_SOURCES}
CFLAGS += -DVAL_TM_VERSION=\"${VAL_TM_VERSION}\" -DVAL_BUILD_CNT=${VAL_BUILD_CNT} \
	-DARCH_${ARCH} -DMCU_${MCU} -DBOARD_${BOARD}

# Linker flags
CFLAGS += -ffunction-sections -fdata-sections -flto
LFLGAS = -Wl,--gc-sections -Wl,-Map=${FILE_TARGET}.map

# output files
FILE_HEX = ${FILE_TARGET}.hex
FILE_ELF = ${FILE_TARGET}.elf

# link
${FILE_TARGET}: ${FILES_OBJ}
	@printf "\n%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${CC} ${CFLAGS} ${LFLAGS} -o ${FILE_ELF} ${FILES_OBJ}
	@printf "\t *.o -> ${FILE_ELF}\n"

# compile
${FILES_OBJ}: ${.TARGET:${PATH_BUILD_TARGET}%.o=${PATH_SOURCES}%.c}
	@printf "\n%sCompilation ...%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s> -> <%s>\n" ${.TARGET:${PATH_BUILD_TARGET}%.o=${PATH_SOURCES}%.c} ${.TARGET}
	@mkdir -p ${.TARGET:H}
	@${CC} ${CFLAGS} ${CFLAGS_${.TARGET:${PATH_BUILD_TARGET}%.o=${PATH_SOURCES}%.c}} \
		-c ${.TARGET:${PATH_BUILD_TARGET}%.o=${PATH_SOURCES}%.c} -o ${.TARGET}

upload: all _mcu_memory_show
#help [avr8] Upload firmware to mcu via Arduino board.
	@printf "\n%sUpload binary to AVR flash, build %i %s\n\n" \
		"${COLOUR_TARGET_INFO}" ${VAL_BUILD_CNT} "${COLOUR_RESET}"
	# FILE_ELF to hex format
	@avr-objcopy -O ihex -R .eeprom ${FILE_ELF} ${FILE_HEX}
	# Upload to Atmega
	avrdude -c ${VAL_PROGRAMMER} -p ${VAL_MCU_SERIAL} -U flash:w:${FILE_HEX}:i -P ${VAL_PROGRAMMER_PORT} -D
.PHONY: upload

# memory usage
_mcu_memory_data:
		@avr-size -G -d ${PATH_BUILD_TARGET}/TaskMate.elf > ${FILE_MEMRAW}

		@awk -v flash_total_k="${VAL_FLASH_SIZE_K}" -v ram_total_k="${VAL_RAM_SIZE_K}" '\
		NR==2 { \
		text  = $$1; \
		data  = $$2; \
		bss   = $$3; \
		flash = text + data; \
		ram   = data + bss; \
		flash_total = flash_total_k * 1024; \
		ram_total = ram_total_k * 1024; \
		flash_pct = (flash / (flash_total)) * 100; \
		ram_pct   = (ram / (ram_total)) * 100; \
		printf("Memory used total %%\n") > "${FILE_MEMDATA}"; \
		printf("Flash %d %d %f\n", flash, flash_total, flash_pct) >> "${FILE_MEMDATA}"; \
		printf("RAM %d %d %f\n", ram, ram_total, ram_pct) >> "${FILE_MEMDATA}"; \
		close("${FILE_MEMDATA}"); \
		}' ${FILE_MEMRAW}
.PHONY: _mcu_memory_data

_mcu_memory_show: _mcu_memory_data
	@printf "\nStatic memory usage : \n"
	@cat ${FILE_MEMRAW}

	@printf "${COLOUR_WHITE_BOLD}Memory usage :\n"
	@awk '\
	NR > 1 { \
		name  = $$1; \
		use = $$2; \
		total = $$3; \
		pct = $$4; \
		printf("\t%-10s : %d / %d bytes (%0.1f%%)\n", name, use, total, pct); \
		}' ${FILE_MEMDATA}
	@printf "${COLOUR_RESET}"
.PHONY: _mcu_memory_show


dump: all
#help [avr8] Disassemble machine code in .hex and .elf
	@printf "\n%sGenerate debugging informations%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	avr-objcopy -O ihex -R .eeprom ${FILE_ELF} ${FILE_HEX}
	avr-objdump -D -m avr6 ${FILE_HEX} > "${PATH_BUILD_TARGET}/hex.txt"
	avr-objdump -D -m avr6 ${FILE_ELF} > "${PATH_BUILD_TARGET}/elf.txt"
.PHONY: dump

tidy_TaskMate:
#help [avr8] tidy static code analysis for TaskMate, configuration /.clang-tidy.
	@printf "\n%sTidy TaskMate static code test%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@clang-tidy $(FILES_SRC) ${FILES_SRC_H} --\
	-I/root/code/TaskMate/TaskMate_current/ \
	-I/root/code/TaskMate/TaskMate_current/${PATH_SOURCES}/ \
	-isystem /usr/local/avr/include \
	-isystem /usr/local/lib/gcc/avr/14.1.0 \
	-D__AVR__=6 -D__AVR_ATmega2560__=1 \
	-DF_CPU=${VAL_CPU_FREQ} \
	-DHAL_SYSTEM_CRITICAL_API_ALLOWED \
	-DAUTOINCLUDE_HAL_SYSTEM_CRITICAL_ALLOWED
.PHONY: tidy_TaskMate

modules_size: all
#help [avr8] List module size sorted from highest.
	@printf "\n%sList module size%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	avr-size -G -d ${PATH_BUILD_TARGET}/TaskMate.elf
	avr-nm --format=bsd --size-sort -r ${PATH_BUILD_TARGET}/TaskMate.elf | head -20
.PHONY: mem_size
