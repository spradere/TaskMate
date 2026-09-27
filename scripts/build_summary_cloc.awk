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
# Description: Show the line count in the build summary.
#
# Inputs: Generated line-count data containing a code_total record.
# Outputs: Formatted lines-of-code value on stdout.
# ------------------------------------------------------------------------------

$1 == "code_total" {
	printf("\t%-16s : %s\n", "lines of code", $2)
}
