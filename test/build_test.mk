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

.PHONY: test_build_system
test_build_system: test_build_configuration .WAIT test_build_scripts .WAIT \
	test_build_guards .WAIT test_build_reports
#help [test] Run the complete build black-box test corpus.
	@printf "\n%sAll build-system tests passed%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

.PHONY: test_build_config
test_build_config: ${SCRIPT_BUILD_TEST}
#help [test] Test Make configuration, target and manifests.
	@${SCRIPT_BUILD_TEST} configuration "." "${PATH_BUILD_BUILD_TEST}"

.PHONY: test_build_scripts
test_build_scripts: ${SCRIPT_BUILD_TEST}
#help [test] Test build-system shell utilities.
	@${SCRIPT_BUILD_TEST} scripts "." "${PATH_BUILD_BUILD_TEST}"

.PHONY: test_build_guards
test_build_guards: ${SCRIPT_BUILD_TEST}
#help [test] Test hardware, header, and architecture guards.
	@${SCRIPT_BUILD_TEST} guards "." "${PATH_BUILD_BUILD_TEST}"

.PHONY: test_build_reports
test_build_reports: ${SCRIPT_BUILD_TEST}
#help [test] Test build log, size, memory, line-count.
	@${SCRIPT_BUILD_TEST} reports "." "${PATH_BUILD_BUILD_TEST}"
