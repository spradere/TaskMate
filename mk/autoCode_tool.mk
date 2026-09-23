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
# autoCode host tool
################################################################################

# Compile autoCode with Clang, independently from the selected hardware compiler
CFLAGS_AUTOCODE = -DAUTOCODE_BUILD -I${PATH_SRCS}/
CFLAGS_AUTOCODE += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

${FILE_AUTOCODE_TARGET}: ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H} ${FILE_ERROR_LEVEL}
	@printf "%sCompiling autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${CFLAGS_AUTOCODE} ${FILES_AUTOCODE_SRC} -o ${FILE_AUTOCODE_TARGET}
