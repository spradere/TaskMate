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
# Print the TaskMate version derived from the closest Git tag
################################################################################

if git_description=$(git describe --tags 2>/dev/null); then
	git_version=${git_description%%-*}
	printf "%s\n" "${git_version#v}"
else
	printf "0.00\n"
fi
