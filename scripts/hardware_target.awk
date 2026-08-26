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
# Check that a hardware target is listed in the supported target configuration
################################################################################

BEGIN {
	expected = normalise(hardware_target)
}

(NF == 0) || (/^[[:space:]]*#/) { next }

{
	candidate = $1
	for (field = 2; field <= NF; field++)
	{
		candidate = candidate " " $field
	}

	if (candidate == expected)
	{
		found = 1
		exit
	}
}

END {
	if (!found)
	{
		printf("Unsupported hardware target: %s\n", hardware_target)
		exit 1
	}
}

function normalise(value)
{
	gsub(/^[[:space:]]+|[[:space:]]+$/, "", value)
	gsub(/[[:space:]]+/, " ", value)
	return value
}
