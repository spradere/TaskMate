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

FILE_LINK_COMPILER = ${CC}
FILE_LINK_FLAGS =
.if !empty(FILES_CPP_SRC)
FILE_LINK_COMPILER = ${CXX}
FILE_LINK_FLAGS = -nostdlib++
.endif

${FILE_TARGET}: ${FILES_OBJ}
	@printf "%sLinking%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${FILE_LINK_COMPILER} ${FILE_LINK_FLAGS} -o "${FILE_TARGET}" ${FILES_OBJ} ${LDFLAGS}
	@printf "\t*.o -> %s\n" "${FILE_TARGET}"

FILE_COMPILE_SRC = ${.TARGET:${PATH_BUILD_TARGET}/%.o=%.c}

${FILES_C_OBJ}: ${FILE_COMPILE_SRC}
	@printf "%sCompilation ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s>\n" ${FILE_COMPILE_SRC}
	@mkdir -p "${.TARGET:H}"
	@${CC} ${CFLAGS} ${CFLAGS_${FILE_COMPILE_SRC}} \
		-c "${FILE_COMPILE_SRC}" -o "${.TARGET}"

FILE_COMPILE_CPP_SRC = ${.TARGET:${PATH_BUILD_TARGET}/%.cpp.o=%.cpp}

${FILES_CPP_OBJ}: ${FILE_COMPILE_CPP_SRC}
	@printf "%sCompilation ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@printf "source : <%s>\n" ${FILE_COMPILE_CPP_SRC}
	@mkdir -p "${.TARGET:H}"
	@${CXX} ${CXXFLAGS} ${CFLAGS_${FILE_COMPILE_CPP_SRC}} \
		-c "${FILE_COMPILE_CPP_SRC}" -o "${.TARGET}"

.PHONY: _mcu_memory_data
_mcu_memory_data: ${FILE_TARGET}
	@printf "Memory used total %%\n" > "${FILE_MEMDATA}"

.PHONY: run
run: all
#help run: [freebsd] Run the ucontext simulation.
	@"./${FILE_TARGET}"
