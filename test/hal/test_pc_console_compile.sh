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
# Description: Compile the PC console with signed-conversion warnings as errors.
#
# Inputs: FreeBSD C compiler and ncurses headers.
# Outputs: Compiler diagnostics and nonzero status on failure.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

cc -std=gnu17 -Wall -Wextra -Wconversion -Wsign-conversion -Werror=sign-conversion \
	-Isrcs -I. -fsyntax-only srcs/hal/board/pc/pc_console.c
