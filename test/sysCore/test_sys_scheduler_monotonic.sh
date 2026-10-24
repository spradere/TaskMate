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
# Description: Check scheduler monotonic timestamps against the test1 contract.
# Inputs: Generated test1 headers and the host C compiler.
# Outputs: Nonzero status if the selected thread gets the wrong start tick.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

cc -std=gnu17 -O0 -Wall -Wextra -Werror \
	-Isrcs -I. -Ibuild/test1_arduinoMega_atmega2560_avr8/generated \
	test/sysCore/test_sys_scheduler_monotonic.c -o "${tmp_dir}/sys_scheduler_monotonic"

"${tmp_dir}/sys_scheduler_monotonic"
