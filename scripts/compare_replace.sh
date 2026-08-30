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
# Compare files and replace if different
################################################################################

shift

printf '%s\n' ${2} > "${1}.tmp"

if ! cmp -s "${1}.tmp" "${1}"; then
	mv "${1}.tmp" "${1}"
else
	rm "${1}.tmp"
fi
