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
# Description: Check Timer3 monotonic driver against a host register model.
# Inputs: Generated test1 headers and the host C compiler.
# Outputs: Nonzero status if timer setup, tick reads, or life cycle regress.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

mkdir -p "${tmp_dir}/avr"
: > "${tmp_dir}/avr/io.h"
printf '%s\n' '#define ISR(vector) void vector(void)' > "${tmp_dir}/avr/interrupt.h"

cc -std=gnu17 -O0 -Wall -Wextra -Werror -DF_CPU=16000000UL \
	-I"${tmp_dir}" -Isrcs -I. \
	-Ibuild/test1_arduinoMega_atmega2560_avr8/generated \
	test/hal/test_at2560_timer_monotonic.c -o "${tmp_dir}/timer_monotonic"

"${tmp_dir}/timer_monotonic"
