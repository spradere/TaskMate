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
# tm_string semantic tests
################################################################################

CFLAGS_TM_STRING_TEST = -std=c17 -Wall -Wextra -Werror -Wno-varargs \
	-fsanitize=address,undefined -fno-omit-frame-pointer -Isrcs

${FILE_TM_STRING_TEST_TARGET}: ${FILE_TM_STRING_TEST} \
								  ${PATH_SRCS}/system/sysCall/sc_string.c \
								  ${PATH_SRCS}/tmLibc/tm_snprintf.c
	@mkdir -p ${PATH_BUILDS}
	@clang ${CFLAGS_TM_STRING_TEST} ${FILE_TM_STRING_TEST} -o ${FILE_TM_STRING_TEST_TARGET}

.PHONY: test_tm_string
test_tm_string: ${FILE_TM_STRING_TEST_TARGET}
#help [test] Test TaskMate string primitives and formatter.
	@${FILE_TM_STRING_TEST_TARGET}
	@printf "%sAll tm_string tests passed%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
