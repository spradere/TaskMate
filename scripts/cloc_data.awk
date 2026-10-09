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
# Description: Calculate source, comment, build, and documentation line counts.
#
# Inputs: Raw cloc report and the output path supplied through the file variable.
# Outputs: Aggregated line-count values written to the requested output file.
# ------------------------------------------------------------------------------

$1 == "C" {
	c_blank += $3
	c_comment += $4
	c_code += $5
	}

($1 == "C/C++") && ($2 == "Header") {
	c_blank += $4
	c_comment += $5
	c_code += $6
	}

($1 == "make") || ($1 == "awk") {
	build_blank += $3
	build_comment += $4
	build_code += $5
	}
	
($1 == "Bourne") && ($2 == "Shell") {
	build_blank += $4
	build_comment += $5
	build_code += $6
	}

($1 == "Markdown") || ($1 == "Text") {
	doc_blank += $3
	doc_code += $5
	}

END {
	loc_total = c_blank + c_comment + c_code + build_blank + build_comment + build_code
	code_total = c_code + build_code
	build_total = build_blank + build_comment + build_code
	comment_total = c_comment + build_comment
	doc_total = doc_blank + doc_code

	code_pct = (code_total / loc_total) * 100
	comment_pct = (comment_total / loc_total) * 100
	doc_pct = (doc_total / (loc_total+doc_total)) * 100
	build_pct = (build_total / loc_total) * 100

	printf("Count lines of code \n") > file
	printf("code_total %d\n", loc_total) >> file
	printf("build_total %d\n", build_total) >> file
	printf("code+doc_total %d\n", loc_total + doc_total) >> file
	printf("code_pct %0.1f\n", code_pct) >> file
	printf("comment_pct %0.1f\n", comment_pct) >> file
	printf("doc_pct %0.1f\n", doc_pct) >> file
	printf("build_pct %0.1f\n", build_pct) >> file

	close(file)
	}
