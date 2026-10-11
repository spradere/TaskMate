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
# Description: Compile the PC console and check libc output in a pseudo terminal.
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

script -eq /dev/null env TERM=xterm LINES=24 COLUMNS=80 \
	"${PATH_TEST_TMP}/test_console" >/dev/null
