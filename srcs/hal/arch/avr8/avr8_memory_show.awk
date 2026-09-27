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
# Description: Show AVR flash and static RAM usage.
#
# Inputs: Generated memory data containing name, used, total, and percentage fields.
# Outputs: Formatted memory usage on stdout.
# ------------------------------------------------------------------------------

NR > 1 {
	name = $1
	used = $2
	total = $3
	pct = $4
	printf("\t%-10s : %d / %d bytes (%0.1f%%)\n", name, used, total, pct)
}
