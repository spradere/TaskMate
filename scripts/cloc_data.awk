# Count lines of code

$1 == "C" {
	c_blank += $3
	c_comment += $4
	c_code += $5
	}

$1 == "C/C++" && $2 == "Header" {
	c_blank += $4
	c_comment += $5
	c_code += $6
	}

($1 == "make") || ($1 == "awk") {
	make_blank += $3
	make_comment += $4
	make_code += $5
	}

($1 == "Markdown") || ($1 == "Text") {
	doc_blank += $3
	doc_code += $5
	}

END {
	loc_total = c_blank + c_comment + c_code + make_blank + make_comment + make_code
	code_total = c_code + make_code
	make_total = make_blank + make_comment + make_code
	comment_total = c_comment + make_comment
	doc_total = doc_blank + doc_code

	code_pct = (code_total / loc_total) * 100
	comment_pct = (comment_total / loc_total) * 100
	doc_pct = (doc_total / loc_total) * 100
	make_pct = (make_total / loc_total) * 100

	printf("Count lines of code \n") > file
	printf("loc_total %d\n", loc_total) >> file
	printf("make_total %d\n", make_total) >> file
	printf("loc+doc %d\n", loc_total + doc_total) >> file
	printf("code_pct %0.1f\n", code_pct) >> file
	printf("comment_pct %0.1f\n", comment_pct) >> file
	printf("doc_pct %0.1f\n", doc_pct) >> file
	printf("make_pct %0.1f\n", make_pct) >> file

	close(file)
	}
