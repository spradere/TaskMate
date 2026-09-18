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
# Check that the autoCode executable API version matches the build expectation
################################################################################

BEGIN {
	major_name = "AC_AUTOCODE_VER_MAJOR"
	minor_name = "AC_AUTOCODE_VER_MINOR"
	failure = 0

	validateExpectedVersion("major", expected_major)
	validateExpectedVersion("minor", expected_minor)
}

($1 == "#define") && (($2 == major_name) || ($2 == minor_name)) {
	if (($2 in version_value) || (NF != 3) || ($3 !~ /^[0-9]+$/))
	{
		printf("Invalid autoCode version definition for %s\n", $2) > "/dev/stderr"
		failure = 1
		next
	}

	version_value[$2] = $3
}

END {
	checkVersion(major_name, expected_major)
	checkVersion(minor_name, expected_minor)
	exit failure
}

function validateExpectedVersion(component, value)
{
	if (value !~ /^[0-9]+$/)
	{
		printf("Invalid expected autoCode %s version: %s\n", component, value) > "/dev/stderr"
		failure = 1
	}
}

function checkVersion(name, expected)
{
	if (!(name in version_value))
	{
		printf("Missing autoCode version definition: %s\n", name) > "/dev/stderr"
		failure = 1
		return
	}

	if (version_value[name] != expected)
	{
		printf("autoCode version mismatch for %s: expected %s, found %s\n", \
			name, expected, version_value[name]) > "/dev/stderr"
		failure = 1
	}
}
