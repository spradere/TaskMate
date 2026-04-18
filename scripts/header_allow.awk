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
# Parse and check allowed system include
################################################################################

BEGIN {
	state = "outside"
	block_id = 0
}

/^[ \t]*$/ { next }

/^[ \t]*#/ { next }

{ line = $0 }


state == "outside" {
	if (line ~ /^[ \t]*\{[ \t]*$/)
	{
		block_id++
		state = "in_block"
		block_has_pattern[block_id] = 0
		block_has_allow[block_id] = 0
		next
	}

	error("unexpected content outside block")
}

state == "in_block" {
	if (line ~ /^[ \t]*\}[ \t]*$/)
	{
		if (!block_has_pattern[block_id])
			error("missing pattern in block")
		if (!block_has_allow[block_id])
			error("missing allow block")
		state = "outside"
		next
	}

	if (line ~ /^[ \t]*pattern[ \t]+[^ \t{}][^{}]*$/)
	{
		if (block_has_pattern[block_id])
			error("duplicate pattern in block")

		pat = line
		sub(/^[ \t]*pattern[ \t]+/, "", pat)
		sub(/[ \t]+$/, "", pat)

		pattern[block_id] = pat
		block_has_pattern[block_id] = 1
		next
	}

	if (line ~ /^[ \t]*allow[ \t]*\{[ \t]*$/)
	{
		if (block_has_allow[block_id])
			error("duplicate allow block")

		state = "in_allow"
		block_has_allow[block_id] = 1
		next
	}

	error("invalid statement inside block <" $1 ">")
}

state == "in_allow" {
	if (line ~ /^[ \t]*\}[ \t]*$/)
	{
		state = "in_block"
		next
	}

	if (line ~ /^[ \t]*\{[ \t]*$/)
		error("unexpected '{' inside allow block")

	file = line
	sub(/^[ \t]+/, "", file)
	sub(/[ \t]+$/, "", file)

	if (file == "") error("empty entry in allow block")

	allow[block_id, src_dir "/" file] = 1
	allow_count[block_id]++
	next
}

END {
	if (failed) exit 1

	if (state == "in_block")
		error_end("missing closing '}' for block")

	if (state == "in_allow")
		error_end("missing closing '}' for allow block")

	for (i = 1; i <= block_id; i++)
	{
		if (!block_has_pattern[i])
			error_end("block " i ": missing pattern")
		if (!block_has_allow[i])
			error_end("block " i ": missing allow block")
		if (allow_count[i] == 0)
			error_end("block " i ": empty allow block")
	}

	scan_failed = 0
	print "Headers allow check report" > h_check_log
	for (i = 1; i <= block_id; i++)
	{
		check_pattern(i)
	}
	if (scan_failed) exit 3
}

function error(msg) {
	failed = 1
	printf("[%s:%d] parse error : %s\n", FILENAME, NR, msg)
	exit 1
}

function error_end(msg) {
	printf("[%s] parse error: %s\n",FILENAME, msg)
	exit 2
}

function trim(t)
{
	sub(/^[ \t]+/, "", t)
	sub(/[ \t]+$/, "", t)
	return t
}

function check_pattern(block, cmd, file, found_any)
{
	found_any = 0

	printf("Checking pattern %s ...\n", pattern[block])
	printf("\nChecking pattern %s ...\n", pattern[block]) > h_check_log

	cmd = "grep -R -l \"" pattern[block] "\" \"" src_dir "\" 2>/dev/null"

	while ((cmd | getline file) > 0)
	{
		found_any = 1
		file = trim(file)

		if ((block, file) in allow)
		{
			printf("[  OK  ] %s\n", file) > h_check_log
		}
		else
		{
			printf("[\033[1;31m FAIL \033[0m] Forbidden include detected in: %s\n", file)
			printf("[ >>>FAIL<<< ] Forbidden include detected in: %s\n", file) > h_check_log
			scan_failed = 1
		}
	}
	close(cmd)

	if (!found_any)
		printf("[ INFO ] No file matched pattern: %s\n", pattern[block])
}
