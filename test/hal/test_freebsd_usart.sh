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
# Description: Compile the FreeBSD USART as C++ and exercise its C ABI from C.
# Inputs: Host C and C++ compilers, generated TaskMate interface contracts.
# Outputs: Compiler diagnostics and nonzero status on failure.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

PATH_TEST_TMP=$(mktemp -d)
trap 'rm -rf "${PATH_TEST_TMP}"' EXIT HUP INT TERM

cc -std=gnu17 -Wall -Wextra -Werror -Isrcs -I. \
	-Ibuild/z600_pc_freebsd_ucontext/generated \
	-c test/hal/test_freebsd_usart.c -o "${PATH_TEST_TMP}/test.o"
c++ -std=c++20 -pedantic-errors -Wall -Wextra -Werror -fno-exceptions -fno-rtti \
	-fno-threadsafe-statics -fno-use-cxa-atexit -Isrcs -I. \
	-Ibuild/z600_pc_freebsd_ucontext/generated \
	-c srcs/hal/host/freebsd/freebsd_usart.cpp -o "${PATH_TEST_TMP}/usart.o"
if nm -u "${PATH_TEST_TMP}/usart.o" | grep -E '_Zn|_Zd|_ZTI|_ZTV|__cxa|__gxx|__dynamic_cast'; then
	printf 'Unexpected C++ runtime dependency in FreeBSD USART\n' >&2
	exit 1
fi
if readelf -SW "${PATH_TEST_TMP}/usart.o" | grep -E '\.(init_array|fini_array|ctors|dtors)'; then
	printf 'Unexpected C++ initialization or destruction in FreeBSD USART\n' >&2
	exit 1
fi
c++ -nostdlib++ "${PATH_TEST_TMP}/test.o" "${PATH_TEST_TMP}/usart.o" \
	-o "${PATH_TEST_TMP}/test_freebsd_usart"
"${PATH_TEST_TMP}/test_freebsd_usart"
