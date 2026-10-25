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
# Description: Create a dated Git bundle containing all repository refs and history.
#
# Inputs: The repository containing this script and the current local date.
# Outputs: A bundle in .backup at the repository root, or a nonzero status on failure.
# ------------------------------------------------------------------------------

repository=$(CDPATH= cd "$(dirname "$0")/.." && pwd) || exit 1
bundle_date=$(date '+%Y_%m_%d') || exit 1
bundle_dir="${repository}/.backup"
bundle_file="${bundle_dir}/TaskMate.bundle_${bundle_date}"

mkdir -p "$bundle_dir" || exit 1

if ! git -C "$repository" bundle create "$bundle_file" --all; then
	printf 'Failed to create Git bundle: %s\n' "$bundle_file" >&2
	exit 1
fi

printf 'Git bundle created: %s\n' "$bundle_file"
