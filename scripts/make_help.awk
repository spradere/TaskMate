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
# Description: Collect and sort documented Make targets.
#
# Inputs: Makefiles containing #help target and description records.
# Outputs: Sorted, width-limited target descriptions on stdout.
# ------------------------------------------------------------------------------

/^#help[[:space:]]+[^:]+:[[:space:]]*/ {
	help = $0
	sub(/^#help[[:space:]]+/, "", help)
	separator_index = index(help, ":")
	target = substr(help, 1, separator_index)
	description = substr(help, separator_index + 1)
	sub(/^[[:space:]]*/, "", description)
	help_list[++help_count] = sprintf("%-21.21s %.57s", target, description)
}

END {
	for( item = 2; item <= help_count; item++ ) {
		temp = help_list[item]
		for( list_index = item;
			list_index > 1 && temp < help_list[list_index - 1]; list_index-- ) {
			help_list[list_index] = help_list[list_index - 1]
		}
		help_list[list_index] = temp
	}
	for( list_index = 1; list_index <= help_count; list_index++ ) {
		print help_list[list_index]
	}
}
