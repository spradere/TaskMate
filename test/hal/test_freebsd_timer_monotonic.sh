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
# Description: Check the FreeBSD monotonic timer against a controlled host clock.
# Inputs: Generated z600 headers and the host C compiler.
# Outputs: Nonzero status if tick, life cycle, or wrap behaviour regresses.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

cc -std=gnu17 -O0 -Wall -Wextra -Werror \
	-Isrcs -I. -Ibuild/z600_pc_freebsd_ucontext/generated \
	test/hal/test_freebsd_timer_monotonic.c -o "${tmp_dir}/timer_monotonic"

"${tmp_dir}/timer_monotonic"
