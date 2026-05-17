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
# Manage TaskMate information file
################################################################################

FNR == 1  {
	file = FILENAME
    tmp = FILENAME ".tmp"
   	printf("") > tmp
   	flag_modified = 0
}

(NF == 0) || (/^[[:space:]]*#/) {
	printf("%s\n", $0) >> tmp
	}

$1 == "tm_version" {
	tm_version_infile = $2
	printf("tm_version %s\n", tm_version_make) >> tmp

	if (tm_version_make < tm_version_infile) {print COLOUR_FAIL "[tm_info.awk] tm_version make < file" COLOUR_RESET}
	if (tm_version_make > tm_version_infile) { flag_modified = 1	}
}

$1 == "build_cnt" {
	build_cnt_infile = $2
	printf("build_cnt %s\n", build_cnt_make) >> tmp

	if (build_cnt_make < tm_version_infile) {print COLOUR_FAIL "[tm_info.awk] build_cnt make < file" COLOUR_RESET}
	if (build_cnt_make > build_cnt_infile) { flag_modified = 1 }
}

END {
	if (flag_modified == 1)
	{
		cmd = sprintf("mv %s %s\n", tmp, file)
		system(cmd)
 	} else {
	cmd = sprintf("rm %s\n", tmp)
	}

	system(cmd)
}
