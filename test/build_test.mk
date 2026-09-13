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
# Build-system black-box tests
################################################################################

test_build_system: test_build_configuration .WAIT test_build_scripts .WAIT \
	test_build_guards .WAIT test_build_reports
#help [test] Run the complete build-system black-box test corpus.
	@printf "\n%sAll build-system tests passed%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.PHONY: test_build_system

test_build_configuration: ${FILE_BUILD_TEST_SCRIPT}
#help [test] Test Make configuration, target composition, and generated manifests.
	@${FILE_BUILD_TEST_SCRIPT} configuration "." "${PATH_BUILD_BUILD_TEST}"
.PHONY: test_build_configuration

test_build_scripts: ${FILE_BUILD_TEST_SCRIPT}
#help [test] Test build-system shell utilities.
	@${FILE_BUILD_TEST_SCRIPT} scripts "." "${PATH_BUILD_BUILD_TEST}"
.PHONY: test_build_scripts

test_build_guards: ${FILE_BUILD_TEST_SCRIPT}
#help [test] Test hardware, header, and architecture guards.
	@${FILE_BUILD_TEST_SCRIPT} guards "." "${PATH_BUILD_BUILD_TEST}"
.PHONY: test_build_guards

test_build_reports: ${FILE_BUILD_TEST_SCRIPT}
#help [test] Test build log, size, memory, line-count, and help reports.
	@${FILE_BUILD_TEST_SCRIPT} reports "." "${PATH_BUILD_BUILD_TEST}"
.PHONY: test_build_reports
