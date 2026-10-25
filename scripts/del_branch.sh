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
# Description: Delete selected local codex/* branches after switching to test.
#
# Inputs: Current Git repository and a confirmation for each local codex/* branch.
# Outputs: Branch deletion results and a nonzero status if switching to test fails.
# ------------------------------------------------------------------------------

if ! git switch test; then
	printf "Cannot switch to the test branch.\n" >&2
	exit 1
fi

git pull

branches=$(git for-each-ref --format='%(refname:short)' refs/heads/codex/) || exit 1

if [ -z "$branches" ]; then
	printf "No local codex/* branches to delete.\n"
	exit 0
fi

for branch in $branches; do
	printf "Delete local branch %s? [y/N] " "$branch"
	IFS= read -r answer || answer=""

	case "$answer" in
		y | Y | yes | YES) git branch -D -- "$branch" ;;
		*) printf "Branch kept: %s\n" "$branch" ;;
	esac
done
