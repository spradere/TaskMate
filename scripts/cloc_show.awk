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
# Show line-count values
################################################################################

NR > 1 {
	vars[$1] = $2;
	}

END {
	split("\
		code_total \
		code+doc_total \
		code_pct \
		comment_pct \
		doc_pct \
		make_total \
		make_pct \
		", order, " ")

	for (i = 1; i <= length(order); i++)
	{
		k = order[i]
		display = k
		suffix = ""

		if (k ~ /_pct$/)
		{
			sub(/_pct$/, "", display)
			suffix = "%"
		}

		if (k ~ /_total$/)
		{
			sub(/_total$/, "", display)
			suffix = "loc"
		}

		if (k in vars)
		{
			printf("\t%-12s : %s %s\n", display, vars[k], suffix)
		}
	}
}
