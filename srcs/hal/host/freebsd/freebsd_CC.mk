################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

################################################################################
# FreeBSD native compiler and linker rules
################################################################################

${FILE_TARGET}: ${FILES_OBJ}
	@printf "%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${CC} ${CFLAGS} -o "${FILE_TARGET}" ${FILES_OBJ} ${LDFLAGS}
	@printf "\t*.o -> %s\n" "${FILE_TARGET}"

FILE_COMPILE_SRC = ${.TARGET:${PATH_BUILD_TARGET}/%.o=%.c}

${FILES_OBJ}: ${FILE_COMPILE_SRC}
	@printf "%sCompilation ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s> -> <%s>\n" \
		"${FILE_COMPILE_SRC}" "${.TARGET}"
	@mkdir -p "${.TARGET:H}"
	@${CC} ${CFLAGS} ${CFLAGS_${FILE_COMPILE_SRC}} \
		-c "${FILE_COMPILE_SRC}" -o "${.TARGET}"

.PHONY: _mcu_memory_data
_mcu_memory_data: ${FILE_TARGET}
	@printf "Memory used total %%\n" > "${FILE_MEMDATA}"

.PHONY: run
run: all
#help [freebsd] Run the ucontext simulation in the current terminal.
	@"./${FILE_TARGET}"
