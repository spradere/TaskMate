#!/bin/sh

################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

# ------------------------------------------------------------------------------
# Description: Compile the PC console and check output and input in a pseudo terminal.
#
# Inputs: FreeBSD C compiler, ncurses headers and the script utility.
# Outputs: Compiler diagnostics and nonzero status on failure.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

cc -std=gnu17 -Wall -Wextra -Wconversion -Wsign-conversion -Werror=sign-conversion \
	-Isrcs -I. -fsyntax-only srcs/hal/board/pc/pc_console.c

PATH_TEST_TMP=$(mktemp -d)
trap 'rm -rf "${PATH_TEST_TMP}"' EXIT HUP INT TERM

cc -std=gnu17 -Wall -Wextra -Wconversion -Wsign-conversion -Werror=sign-conversion \
	-Isrcs -I. test/hal/test_pc_console_stdout.c srcs/hal/board/pc/pc_console.c \
	srcs/hal/host/freebsd/freebsd_halt.c -lncursesw -ltinfow -o "${PATH_TEST_TMP}/test_console"

cc -std=gnu17 -Wall -Wextra -Wconversion -Wsign-conversion -Werror=sign-conversion \
	-Isrcs -I. -Dwrefresh=test_wrefresh -c srcs/hal/board/pc/pc_console.c \
	-o "${PATH_TEST_TMP}/pc_console_input.o"
cc -std=gnu17 -Wall -Wextra -Wconversion -Wsign-conversion -Werror=sign-conversion \
	-Isrcs -I. test/hal/test_pc_console_input.c "${PATH_TEST_TMP}/pc_console_input.o" \
	-lncursesw -ltinfow -o "${PATH_TEST_TMP}/test_console_input"

script -eq /dev/null env TERM=xterm LC_ALL=C LINES=24 COLUMNS=80 \
	"${PATH_TEST_TMP}/test_console" >/dev/null
if ! script -eq /dev/null env TERM=xterm LC_ALL=C LINES=24 COLUMNS=80 \
	"${PATH_TEST_TMP}/test_console_input" >"${PATH_TEST_TMP}/input.log"; then
	tail -c 1000 "${PATH_TEST_TMP}/input.log" >&2
	exit 1
fi
if script -eq /dev/null env TERM=xterm LC_ALL=C LINES=24 COLUMNS=80 \
	"${PATH_TEST_TMP}/test_console_input" halt >/dev/null; then
	printf 'F10 did not halt the console\n' >&2
	exit 1
else
	[ "$?" -eq 42 ]
fi
