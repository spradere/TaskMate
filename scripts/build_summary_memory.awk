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
# Show memory usage and thresholds in the build summary
################################################################################

NR > 1 {
	name = $1
	pct = $4
	printf("\t%-16s : %0.1f%%\n", name, pct)

	if ($4 > 98)
	{
		printf("\t>>> ERROR: usage high > 98%% <<<\n")
		exit 1
	}

	if ($4 > 85)
	{
		printf("\t>>> WARNING: usage high > 85%% <<<\n")
	}
}
