# cloc show values

NR > 1 {
	vars[$1] = $2;
	}

END {
	split("\
		loc_total \
		loc+doc \
		code_pct \
		comment_pct \
		make_total \
		make_pct \
		doc_pct" \
		, order, " ")

	for (i = 1; i <= length(order); i++)
	{
		k = order[i]
		if (k in vars)
		{
			printf("\t%-12s : %s\n", k, vars[k])
		}
	}
}
