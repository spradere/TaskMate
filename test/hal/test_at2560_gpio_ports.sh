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
# Description: Check that every ATmega2560 GPIO port has three non-null registers.
# Inputs: Generated test1 headers and the AVR compiler tools.
# Outputs: Nonzero status if the port table is incomplete.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

tmp_dir=$(mktemp -d)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

avr-gcc -mmcu=atmega2560 -std=gnu17 -O0 -Isrcs -I. \
	-Ibuild/test1_arduinoMega_atmega2560_avr8/generated \
	-c srcs/hal/mcu/atmega2560/at2560_gpio.c -o "${tmp_dir}/gpio.o"

symbol=$(avr-nm -S --defined-only "${tmp_dir}/gpio.o" |
	awk '$4 == "mcu_ports" { print $1, $2 }')
if [ -z "${symbol}" ]; then
	printf '%s\n' 'mcu_ports symbol not found' >&2
	exit 1
fi
set -- ${symbol}
offset=$(printf '%d' "0x$1")
size=$(printf '%d' "0x$2")

# Eleven physical ports, with three two-byte register addresses per port.
expected_size=$((11 * 3 * 2))
if [ "${size}" -ne "${expected_size}" ]; then
	printf 'mcu_ports has %s bytes, expected %s\n' "${size}" "${expected_size}" >&2
	exit 1
fi

avr-objcopy --dump-section ".rodata=${tmp_dir}/rodata.bin" "${tmp_dir}/gpio.o"
od -An -v -tx1 -j "${offset}" -N "${size}" "${tmp_dir}/rodata.bin" |
	awk -v expected_bytes="${size}" '
		{
			for (i = 1; i <= NF; i++) {
				if (byte_count % 2 == 0) { low = $i }
				else if ((low == "00") && ($i == "00")) {
					printf "null register address at table field %d\n", byte_count / 2 > "/dev/stderr"
					bad = 1
				}
				byte_count++
			}
		}
		END {
			if (byte_count != expected_bytes) {
				printf "read %d bytes, expected %d\n", byte_count, expected_bytes > "/dev/stderr"
				bad = 1
			}
			if (bad) { exit 1 }
		}'
