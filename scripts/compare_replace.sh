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
# Description: Replace a text file only when its requested content changes.
#
# Inputs: Destination path as $1 and replacement text as $2.
# Outputs: Creates or updates the destination when the requested content differs.
# ------------------------------------------------------------------------------

printf '%s\n' "${2}" > "${1}.tmp"

if ! cmp -s "${1}.tmp" "${1}"; then
	mv "${1}.tmp" "${1}"
else
	rm "${1}.tmp"
fi
