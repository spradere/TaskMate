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
#help run: [freebsd] Run the ucontext simulation.
	@"./${FILE_TARGET}"

VAL_CLANG_TIDY_TARGET != ${CC} -dumpmachine
CFLAGS_CLANG_TIDY = --target=${VAL_CLANG_TIDY_TARGET} -std=gnu17
CFLAGS_CLANG_TIDY += ${CFLAGS:M-D*} ${CFLAGS:M-I*}
CFLAGS_CLANG_TIDY += -include ${FILE_HAL_ARCHITECTURE_TYPES}

OPT_CPPCHECK_TARGET = --platform=unix64 --library=bsd --library=posix
# ucontext is intentional, and cppcheck does not model hal_halt() as non-returning.
OPT_CPPCHECK_TARGET += --suppress=getcontextCalled --suppress=makecontextCalled
OPT_CPPCHECK_TARGET += --suppress=nullPointerRedundantCheck:${PATH_FREEBSD}/freebsd_timerSched.c
CFLAGS_CPPCHECK = ${CFLAGS:M-D*} ${CFLAGS:M-I*}
CFLAGS_CPPCHECK += --include=${FILE_HAL_ARCHITECTURE_TYPES}
CFLAGS_CPPCHECK += --include=${FILE_HAL_STRING_MACRO}

.PHONY: tidy_freebsd
tidy_freebsd: _autocode
#help tidy_freebsd: [freebsd] clang-tidy static code analysis for the FreeBSD host.
	@printf "%sTidy FreeBSD static test code, config in clang-tidy%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.for file in ${FILES_COMPILE_SRC}
	@${VAL_CLANG_TIDY} "${file}" -- ${CFLAGS_CLANG_TIDY} ${CFLAGS_${file}}
.endfor
