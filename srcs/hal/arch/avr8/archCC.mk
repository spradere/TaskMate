################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# output files
FILE_HEX = ${FILE_TARGET}.hex
FILE_ELF = ${FILE_TARGET}.elf

# link
${FILE_TARGET}: ${FILES_OBJ}
	@printf "\n%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${CC} ${CFLAGS} ${LDFLAGS} -o ${FILE_ELF} ${FILES_OBJ}
	@printf "\t *.o -> ${FILE_ELF}\n"

# compiler wrapper
FILE_COMPILE_SRC = ${.TARGET:${PATH_BUILD_TARGET}/%.o=%.c}
# compile
${FILES_OBJ}: ${FILE_COMPILE_SRC}
	@printf "\n%sCompilation ...%s\n\n" \
	    "${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s> -> <%s>\n" \
	    "${FILE_COMPILE_SRC}" "${.TARGET}"
	@mkdir -p "${.TARGET:H}"
	@${CC} ${CFLAGS} ${CFLAGS_${FILE_COMPILE_SRC}} \
	    -c "${FILE_COMPILE_SRC}" -o "${.TARGET}"

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
		@avr-size -G -d ${FILE_ELF} > ${FILE_MEMRAW}

		@awk -v flash_total_k="${VAL_FLASH_SIZE_K}" -v ram_total_k="${VAL_RAM_SIZE_K}" \
			-v output_file="${FILE_MEMDATA}" -f ${PATH_SCRIPTS}/avr_memory_data.awk \
			"${FILE_MEMRAW}"
.PHONY: _mcu_memory_data

_mcu_memory_show: _mcu_memory_data
	@printf "\nStatic memory usage : \n"
	@cat ${FILE_MEMRAW}

	@printf "${COLOUR_WHITE_BOLD}Memory usage :\n"
	@awk -f ${PATH_SCRIPTS}/avr_memory_show.awk "${FILE_MEMDATA}"
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
	@clang-tidy19 $(FILES_SRC) ${FILES_SRC_H} --\
	-I/root/code/TaskMate/current/ \
	-I/root/code/TaskMate/current/${PATH_SRCS}/ \
	-isystem /usr/local/avr/include \
	-isystem /usr/local/lib/gcc/avr/14.2.0 \
	-D__AVR__=6 -D__AVR_ATmega2560__=1 \
	-DF_CPU=${VAL_CPU_FREQ} ${CFLAGS} ${LDFLAGS}

.PHONY: tidy_TaskMate

modules_size: all
#help [avr8] List module size sorted from highest.
	@printf "\n%sList module size%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	avr-size -G -d ${PATH_BUILD_TARGET}/TaskMate.elf
	avr-nm --format=bsd --size-sort -r ${PATH_BUILD_TARGET}/TaskMate.elf | head -20
.PHONY: mem_size
