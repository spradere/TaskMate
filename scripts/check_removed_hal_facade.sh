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

set -u

if [ "$#" -ne 1 ]; then
	printf "Usage: %s <project-root>\n" "$0" >&2
	exit 2
fi

PATH_PROJECT=$1
PATH_REMOVED="$PATH_PROJECT/srcs/hal/public"

if [ -e "$PATH_REMOVED" ]; then
	printf "Removed HAL public directory still exists: %s\n" "$PATH_REMOVED" >&2
	exit 1
fi

for FILE_REMOVED in \
	srcs/hal/arch/avr8/avr8_interrupt.h \
	srcs/hal/arch/avr8/avr8_stack.h
do
	if [ -e "$PATH_PROJECT/$FILE_REMOVED" ]; then
		printf "Removed HAL relay still exists: %s\n" "$FILE_REMOVED" >&2
		exit 1
	fi
done

VAL_LEGACY_PATTERN='hal/public|HAL_PUBLIC_|'
VAL_LEGACY_PATTERN="${VAL_LEGACY_PATTERN}"'#[[:space:]]*define[[:space:]]+'
VAL_LEGACY_PATTERN="${VAL_LEGACY_PATTERN}"'HAL_(CONTEXT|STACK|INT|ARCHITECTURE_TYPES)'
VAL_LEGACY_PATTERN="${VAL_LEGACY_PATTERN}"'([[:space:]]|$)'

for PATH_ACTIVE in srcs mk conf Makefile
do
	if [ ! -e "$PATH_PROJECT/$PATH_ACTIVE" ]; then
		continue
	fi
	if grep -R -n -E \
		"${VAL_LEGACY_PATTERN}" \
		"$PATH_PROJECT/$PATH_ACTIVE"
	then
		printf "Legacy HAL public token found in active project files.\n" >&2
		exit 1
	fi
done

printf "HAL public removal check passed.\n"
