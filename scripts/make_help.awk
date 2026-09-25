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
	target = $1
}

$1 == "#help" {
	description = $0
	sub(/^#help /, "", description)
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
