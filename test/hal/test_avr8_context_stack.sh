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
# Description: Compare AVR context stack operations with the private context layout.
# Inputs: AVR compiler tools, test1 firmware, and AVR8 context sources.
# Outputs: Nonzero status if source macros or linked instructions disagree with the layout.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

if [ "$#" -eq 0 ]; then
	bmake TARGET=test1
	file_elf=$(bmake -V FILE_ELF TARGET=test1)
else
	file_elf=$1
fi

if [ ! -f "${file_elf}" ]; then
	printf 'firmware ELF not found: %s\n' "${file_elf}" >&2
	exit 1
fi

vector=$(printf '#include <avr/interrupt.h>\nTIMER1_COMPA_vect\n' |
	avr-gcc -mmcu=atmega2560 -E -P -x c - | tr -d '\r' | tail -n 1)
case "${vector}" in
	__vector_[0-9]*) ;;
	*) printf 'cannot resolve TIMER1_COMPA_vect: %s\n' "${vector}" >&2; exit 1 ;;
esac

mkdir -p build
tmp_dir=$(mktemp -d build/avr8_context_stack.XXXXXX)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

avr-objdump -d --disassemble="${vector}" "${file_elf}" |
	tr -d '\r' > "${tmp_dir}/vector.txt"
start_symbol=main
if avr-nm --defined-only "${file_elf}" | tr -d '\r' |
	awk '$3 == "hal_contextStart" { found = 1 } END { exit !found }'; then
	start_symbol=hal_contextStart
fi
avr-objdump -d --disassemble="${start_symbol}" "${file_elf}" |
	tr -d '\r' > "${tmp_dir}/start.txt"

awk -f test/hal/avr8_context_stack.awk \
	srcs/hal/arch/avr8/avr8_context.c \
	srcs/hal/arch/avr8/avr8_context.h \
	"${tmp_dir}/vector.txt" "${tmp_dir}/start.txt"
