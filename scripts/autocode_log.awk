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
# Show and append the relevant autoCode log entries
################################################################################

$1 == "[fileUtility.c]" {
	if ($4 ~ /^\*/)
	{
		temp = $0
		sub(/^[^*]*/, "", temp)
		print temp
	}
}

$4 == "keep" {
	temp = $0
	sub(/^[^:]*: /, "", temp)
	print COLOUR_CYAN, temp, COLOUR_RESET
	print temp >> log_file
}

$4 == "change" {
	temp = $0
	sub(/^[^:]*: /, "", temp)
	print COLOUR_YELLOW, temp, COLOUR_RESET
	print temp >> log_file
}
