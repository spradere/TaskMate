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
# Description: Check SCLI tokenization and line assembly using the test1 contract.
# Inputs: Generated test1 headers and the host C compiler.
# Outputs: Nonzero status if SCLI input processing fails.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

for test_name in index receive; do
	cc -std=gnu17 -O0 -Wall -Wextra -Werror -ffunction-sections -fdata-sections \
		-Isrcs -I. -Ibuild/test1_arduinoMega_atmega2560_avr8/generated \
		"test/services/test_scli_${test_name}.c" -Wl,--gc-sections -o "${tmp_dir}/scli_${test_name}"
	"${tmp_dir}/scli_${test_name}"
done
