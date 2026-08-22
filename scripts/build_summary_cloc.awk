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
# Show the line count in the build summary
################################################################################

$1 == "code_total" {
	printf("\t%-16s : %s\n", "lines of code", $2)
}
