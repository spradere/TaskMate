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
# Description: Check ATmega2560 GPIO driver life cycle against host registers.
# Inputs: Generated test1 headers and the host C compiler.
# Outputs: Nonzero status if the driver changes a stopped pin or skips startup.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

mkdir -p "${tmp_dir}/avr"
: > "${tmp_dir}/avr/io.h"

cc -std=gnu17 -O0 -Wall -Wextra -Werror \
	-I"${tmp_dir}" -Isrcs -I. \
	-Ibuild/test1_arduinoMega_atmega2560_avr8/generated \
	test/hal/test_at2560_gpio_lifecycle.c -o "${tmp_dir}/gpio_lifecycle"

"${tmp_dir}/gpio_lifecycle"
