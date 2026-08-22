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
# Show documented Make targets
################################################################################

/^([A-Za-z0-9][A-Za-z0-9_-]*):/ {
	print COLOUR_HELP_TARGET $1 COLOUR_RESET
}

$1 == "#help" {
	printf("  %s%s%s", COLOUR_HELP_TAG, $2, COLOUR_RESET)
	temp = $0
	sub(/.*\]/, "", temp)
	print temp
}
