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
# Extract one source option from init.rc module records
################################################################################

BEGIN {
	if ((source_option != "-source_file") && (source_option != "-source_dir"))
	{
		printf("Unsupported init.rc source option: %s\n", source_option) > "/dev/stderr"
		exit 2
	}
}

{
	for (field = 1; field <= NF; field++)
	{
		if ($field == "#")
		{
			break
		}
		if ($field != source_option)
		{
			continue
		}
		if ((field == NF) || ($(field + 1) ~ /^-/))
		{
			printf("Missing value for %s [%s:%d]\n", source_option, FILENAME, FNR) \
				> "/dev/stderr"
			have_error = 1
			continue
		}

		path = source_root "/" $(field + 1)
		if (!seen[path])
		{
			print path
			seen[path] = 1
		}
		field++
	}
}

END {
	if (have_error)
	{
		exit 2
	}
}
