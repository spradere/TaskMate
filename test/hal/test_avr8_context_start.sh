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
# Description: Keep the initial context address live through AVR link-time optimization.
# Inputs: AVR compiler tools and the AVR8 context implementation.
# Outputs: Nonzero status if the caller's context is optimized away.
# ------------------------------------------------------------------------------

set -eu

PATH_PROJECT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
cd "${PATH_PROJECT}"

mkdir -p build
tmp_dir=$(mktemp -d build/avr8_context_start.XXXXXX)
trap 'rm -rf "${tmp_dir}"' EXIT HUP INT TERM

cat > "${tmp_dir}/context_start.c" <<'EOF'
#include <stdint.h>

#include "hal/arch/avr8/avr8_types.h"

#define TEST_TIMER_STATE_RUNNING 2U

static hal_context_t test_context;

int main(void)
{
	volatile uint8_t timer_state = TEST_TIMER_STATE_RUNNING;
	if( timer_state != TEST_TIMER_STATE_RUNNING ) { return 1; }
	hal_contextStart(&test_context);
}
EOF

context_c=${1:-srcs/hal/arch/avr8/avr8_context.c}
context_start_s=${2:-srcs/hal/arch/avr8/avr8_contextStart.S}

avr-gcc -mmcu=atmega2560 -std=gnu17 -Os -flto -ffunction-sections -fdata-sections \
	-Wl,--gc-sections -Isrcs -Isrcs/hal/arch/avr8 -I. \
	"${tmp_dir}/context_start.c" "${context_c}" "${context_start_s}" \
	-o "${tmp_dir}/context_start.elf"

if ! avr-nm --defined-only "${tmp_dir}/context_start.elf" |
	awk '$3 ~ /^test_context/ { found = 1 } END { exit !found }'; then
	printf '%s\n' 'initial context address was discarded by link-time optimization' >&2
	exit 1
fi
