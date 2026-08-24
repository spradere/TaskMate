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

################################################################################
# Check that every program listed in a file is available
################################################################################

if [ "$#" -ne 1 ]; then
	printf "Usage: %s <programs-list>\n" "$0" >&2
	exit 2
fi

programs_file=$1

if [ ! -r "$programs_file" ]; then
	printf "Programs list is not readable: %s\n" "$programs_file" >&2
	exit 2
fi

missing_programs=""

while IFS= read -r program || [ -n "$program" ]; do
	case "$program" in
		"" | \#*) continue ;;
	esac

	if ! command -v "$program" >/dev/null 2>&1; then
		missing_programs="${missing_programs} ${program}"
	fi
done < "$programs_file"

if [ -n "$missing_programs" ]; then
	printf "Missing required programs:%s\n" "$missing_programs" >&2
	exit 1
fi
