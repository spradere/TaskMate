#!/bin/sh

################################################################################
#
# TaskMate Project
# (c) 2026 PRADERE Sebastien
#
# This file is part of TaskMate and is distributed under the BSD-2-Clause License.
# See the LICENSE file for full license terms.
#
################################################################################

set -u

VAL_STAGE=$1
FILE_AUTOCODE=$2
PATH_WORK_ROOT=$3
VAL_TEST_COUNT=0

writeInitrcVersion()
{
	printf '%s\n' 'setVersion major 1' 'setVersion minor 9'
}

fail()
{
	printf 'autoCode test failure: %s\n' "$1" >&2
	exit 1
}

stageBegin()
{
	PATH_STAGE_WORK="${PATH_WORK_ROOT}/$1"
	if [ -d "${PATH_STAGE_WORK}" ]; then
		find "${PATH_STAGE_WORK}" -depth -delete || fail "cannot clean ${PATH_STAGE_WORK}"
	fi
	mkdir -p "${PATH_STAGE_WORK}" || fail "cannot create ${PATH_STAGE_WORK}"
	printf '\nautoCode test stage: %s\n' "$1"
}

writeTags()
{
	FILE_TAGS=$1
	: > "${FILE_TAGS}"
	for VAL_TAG in thread_stacks threads_alloc drivers_alloc thread_name_catalog \
		driver_name_catalog error_enum error_catalog modules_count modules_list gpio_signals wire_gpio \
		scli_commands
	do
		printf '%s\n%s\n%s\n' "// [autoCode_tag] ${VAL_TAG}" \
			"stale generated data" "// [/tag]" >> "${FILE_TAGS}"
	done
}

writeConfig()
{
	printf '%s\n' \
		"--error_count 1000" \
		"--errors ${PATH_CASE}/errors.list" \
		"--initrc ${PATH_CASE}/initrc.list" \
		"--parsetag ${PATH_CASE}/tags.list" \
		"--gpio_signals ${PATH_CASE}/signals.gpio" \
		"--wire_gpio ${PATH_CASE}/sources/targetWireSignal.c" \
		"--generated_path ${PATH_CASE}/generated" \
		"--source_path ${PATH_CASE}/sources" > "${PATH_CASE}/autoCode.conf"
}

caseBegin()
{
	PATH_CASE="${PATH_STAGE_WORK}/$1"
	mkdir -p "${PATH_CASE}" || fail "cannot create ${PATH_CASE}"
	mkdir -p "${PATH_CASE}/generated" || fail "cannot create ${PATH_CASE}/generated"
	mkdir -p "${PATH_CASE}/sources/commands" \
		"${PATH_CASE}/sources/system/services/commands" || \
		fail "cannot create source fixtures"
	printf "%s\n" "void fixture(void) {}" > "${PATH_CASE}/sources/system.c"
	: > "${PATH_CASE}/sources/system/services/commands/date.h"
	: > "${PATH_CASE}/sources/system/services/commands/driver.h"
	printf "%s\n" "static void targetWireSignal(void) {}" \
		> "${PATH_CASE}/sources/targetWireSignal.c"
	printf '%s\n' 'ERR_TEST "" FLOW' > "${PATH_CASE}/errors.err"
	printf '%s\n' "${PATH_CASE}/errors.err" > "${PATH_CASE}/errors.list"
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	printf '%s\n' "${PATH_CASE}/init.rc" > "${PATH_CASE}/initrc.list"
	writeTags "${PATH_CASE}/tags.c"
	printf '%s\n' "${PATH_CASE}/tags.c" > "${PATH_CASE}/tags.list"
	printf '%s\n' 'GPIO_SIGNAL_TEST' > "${PATH_CASE}/signals.gpio"
	writeConfig
}

logContains()
{
	if ! grep -F -q -- "$2" "${PATH_STAGE_WORK}/$1.log"; then
		fail "$1: missing diagnostic <$2>"
	fi
}

logDoesNotContain()
{
	if grep -F -q -- "$2" "${PATH_STAGE_WORK}/$1.log"; then
		fail "$1: unexpected diagnostic <$2>"
	fi
}

expectFailure()
{
	VAL_NAME=$1
	VAL_PATTERN=$2
	shift 2
	if "$@" > "${PATH_STAGE_WORK}/${VAL_NAME}.log" 2>&1; then
		fail "${VAL_NAME}: command unexpectedly succeeded"
	fi
	logContains "${VAL_NAME}" "${VAL_PATTERN}"
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
}

expectSuccess()
{
	VAL_NAME=$1
	shift
	if ! "$@" > "${PATH_STAGE_WORK}/${VAL_NAME}.log" 2>&1; then
		fail "${VAL_NAME}: command failed"
	fi
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
}

assertNoTemporaryFiles()
{
	if find "${PATH_STAGE_WORK}" -name '*.tmp' -print | grep -q .; then
		fail "temporary autoCode files remain in ${PATH_STAGE_WORK}"
	fi
}

runCommandLineTests()
{
	stageBegin command_line
	expectFailure no_argument "autoCode bad argc" "${FILE_AUTOCODE}"
	expectFailure too_many_arguments "autoCode bad argc" \
		"${FILE_AUTOCODE}" unused.conf extra.conf
}

runOptionFailure()
{
	expectFailure "$1" "$2" "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
}

runOptionTests()
{
	stageBegin options
	expectFailure missing_configuration "opening file" \
		"${FILE_AUTOCODE}" "${PATH_STAGE_WORK}/missing.conf"

	caseBegin wrong_token_count
	printf '%s\n' '--error_count' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure wrong_token_count "wrong token count"

	caseBegin unknown_option
	printf '%s\n' '--unknown value' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure unknown_option "unknown option"

	caseBegin all_required_missing
	: > "${PATH_CASE}/autoCode.conf"
	runOptionFailure all_required_missing \
		"required autoCode option --error_count is not set"
	logContains all_required_missing "required autoCode option --gpio_signals is not set"
	logContains all_required_missing "required autoCode option --wire_gpio is not set"
	logContains all_required_missing "required autoCode option --source_path is not set"

	caseBegin all_required_duplicate
	cp "${PATH_CASE}/autoCode.conf" "${PATH_CASE}/duplicate.conf"
	cat "${PATH_CASE}/duplicate.conf" >> "${PATH_CASE}/autoCode.conf"
	runOptionFailure all_required_duplicate \
		"required autoCode option --error_count is multiple set"
	logContains all_required_duplicate "required autoCode option --gpio_signals is multiple set"
	logContains all_required_duplicate "required autoCode option --wire_gpio is multiple set"
	logContains all_required_duplicate "required autoCode option --source_path is multiple set"

	for VAL_VALUE in -1 invalid 1x 4294967296
	do
		VAL_NAME=$(printf '%s' "${VAL_VALUE}" | tr -c '[:alnum:]' '_')
		caseBegin "invalid_error_count_${VAL_NAME}"
		sed "s/--error_count 1000/--error_count ${VAL_VALUE}/" \
			"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
		mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
		runOptionFailure "invalid_error_count_${VAL_NAME}" "invalid --error_count value"
	done

	caseBegin invalid_source_path
	sed "s|--source_path .*|--source_path ${PATH_CASE}/sources/system.c|" \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	runOptionFailure invalid_source_path "invalid --source_path directory"

	caseBegin unterminated_option
	printf '%s\n' '--error_count "1000' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure unterminated_option "unterminated string"

	caseBegin long_option_line
	awk 'BEGIN { printf "--errors "; for (i = 0; i < 260; i++) printf "x"; \
		printf "\n" }' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure long_option_line "reading file"
}

runErrorTests()
{
	stageBegin errors
	caseBegin missing_error_list
	sed "s|${PATH_CASE}/errors.list|${PATH_CASE}/missing.list|" \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	expectFailure missing_error_list "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_error_file
	printf '%s\n' "${PATH_CASE}/missing.err" > "${PATH_CASE}/errors.list"
	expectFailure missing_error_file "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin stop_after_error_file_failure
	printf '%s\n' "${PATH_CASE}/missing.err" "${PATH_CASE}/must_not_be_opened.err" \
		> "${PATH_CASE}/errors.list"
	printf '%s\n' 'ERR_NOT_REACHED "not reached" WARN' \
		> "${PATH_CASE}/must_not_be_opened.err"
	expectFailure stop_after_error_file_failure "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logDoesNotContain stop_after_error_file_failure "must_not_be_opened.err"

	caseBegin malformed_errors
	printf '%s\n' \
		'ERR_FIELDS "message"' \
		'ERR_DUPLICATE "first" WARN' \
		'ERR_DUPLICATE "second" WARN' \
		'ERR_FLOW_MESSAGE "must be empty" FLOW' \
		'ERR_LEVEL "message" INVALID' \
		'ERR_UNTERMINATED "message WARN' > "${PATH_CASE}/errors.err"
	VAL_INDEX=0
	while [ "${VAL_INDEX}" -le 256 ]
	do
		printf 'ERR_LIMIT_%03d "message" WARN\n' "${VAL_INDEX}" \
			>> "${PATH_CASE}/errors.err"
		VAL_INDEX=$((VAL_INDEX + 1))
	done
	expectFailure malformed_errors "wrong token count != 3" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	for VAL_PATTERN in "Duplicate error name" "FLOW error message must be empty" \
		"wrong error level argument" "unterminated string" "Too many errors"
	do
		logContains malformed_errors "${VAL_PATTERN}"
	done

	caseBegin long_error_line
	awk 'BEGIN { printf "ERR_LONG \""; for (i = 0; i < 260; i++) printf "x"; \
		printf "\" WARN\n" }' > "${PATH_CASE}/errors.err"
	expectFailure long_error_line "reading file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin long_error_list_line
	awk 'BEGIN { for (i = 0; i < 260; i++) printf "x"; printf "\n" }' \
		> "${PATH_CASE}/errors.list"
	expectFailure long_error_list_line "reading file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
}

runInitrcTests()
{
	stageBegin initrc
	caseBegin missing_initrc_list
	sed "s|${PATH_CASE}/initrc.list|${PATH_CASE}/missing.list|" \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	expectFailure missing_initrc_list "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_initrc_file
	printf '%s\n' "${PATH_CASE}/missing.rc" > "${PATH_CASE}/initrc.list"
	expectFailure missing_initrc_file "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin stop_after_initrc_file_failure
	printf '%s\n' "${PATH_CASE}/missing.rc" "${PATH_CASE}/must_not_be_opened.rc" \
		> "${PATH_CASE}/initrc.list"
	writeInitrcVersion > "${PATH_CASE}/must_not_be_opened.rc"
	expectFailure stop_after_initrc_file_failure "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logDoesNotContain stop_after_initrc_file_failure "must_not_be_opened.rc"

	caseBegin valid_commands
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' \
		'addModule driver driver -run driver -i2c 0x20 -source_file system.c' \
		'addModule service system -run service -stack 256 -source_file system.c' \
		'addModule task task -run user -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_commands "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_source_option
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf "%s\n" "addModule service system -run core -stack 256" \
		>> "${PATH_CASE}/init.rc"
	expectFailure missing_source_option "-source_file or -source_dir option is not set" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin invalid_source_file
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf "%s\n" "addModule service system -run core -stack 256 -source_file missing.c" \
		>> "${PATH_CASE}/init.rc"
	expectFailure invalid_source_file "unknown data" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin invalid_source_dir
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf "%s\n" "addModule service system -run core -stack 256 -source_dir system.c" \
		>> "${PATH_CASE}/init.rc"
	expectFailure invalid_source_dir "unknown data" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin valid_source_dir
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf "%s\n" "addModule service system -run core -stack 256 -source_dir commands" \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_source_dir "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin valid_multiple_source_files
	printf '%s\n' 'void commandFixture(void) {}' > "${PATH_CASE}/sources/commands/date.c"
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf "%s%s\n" \
		"addModule service system -run core -stack 256 -source_file system.c" \
		" -source_file commands/date.c" \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_multiple_source_files "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin valid_mixed_sources
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf "%s%s\n" \
		"addModule service system -run core -stack 256 -source_file system.c" \
		" -source_dir commands" \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_mixed_sources "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin valid_scli_commands
	printf '%s\n' 'addScliCommand date' 'addScliCommand driver' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_scli_commands "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin invalid_scli_command_count
	printf '%s\n' 'addScliCommand date dateCommand' >> "${PATH_CASE}/init.rc"
	expectFailure invalid_scli_command_count "addScliCommand token count" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin invalid_scli_command_identifier
	printf '%s\n' 'addScliCommand bad/name' >> "${PATH_CASE}/init.rc"
	expectFailure invalid_scli_command_identifier "invalid SCLI command identifier" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin unknown_scli_command
	printf '%s\n' 'addScliCommand missing' >> "${PATH_CASE}/init.rc"
	expectFailure unknown_scli_command "unknown SCLI command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_scli_command_header
	find "${PATH_CASE}/sources/system/services/commands/date.h" -delete
	printf '%s\n' 'addScliCommand date' >> "${PATH_CASE}/init.rc"
	expectFailure missing_scli_command_header "SCLI command header not found" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin long_scli_command_identifier
	printf '%s\n' 'addScliCommand abcdefghijklmnopqrstuvwxyzabcdef' \
		>> "${PATH_CASE}/init.rc"
	expectFailure long_scli_command_identifier "SCLI command identifier is too long" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin duplicate_scli_command
	printf '%s\n' 'addScliCommand date' 'addScliCommand date' \
		>> "${PATH_CASE}/init.rc"
	expectFailure duplicate_scli_command "duplicate SCLI command name" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_stack
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure missing_stack "-stack option is not set" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	for VAL_STACK in 0 2 invalid 65536
	do
		VAL_NAME=$(printf '%s' "${VAL_STACK}" | tr -c '[:alnum:]' '_')
		caseBegin "invalid_stack_${VAL_NAME}"
		writeInitrcVersion > "${PATH_CASE}/init.rc"
		printf '%s\n' \
			"addModule service system -run core -stack ${VAL_STACK} -source_file system.c" \
			>> "${PATH_CASE}/init.rc"
		expectFailure "invalid_stack_${VAL_NAME}" "unknown data" \
			"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	done

	caseBegin multiple_stack
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' \
		'addModule service system -run core -stack 256 -stack 512 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure multiple_stack "-stack option is multiple set" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin driver_stack
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule driver driver -run core -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure driver_stack "-stack option is not valid for driver modules" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_initrc_version
	printf '%s\n' 'addModule service system -run core -stack 256' > "${PATH_CASE}/init.rc"
	expectFailure missing_initrc_version "first init.rc command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin legacy_initrc_version
	printf '%s\n' '!set_initrc_ver_major 1' '!set_initrc_ver_minor 6' \
		> "${PATH_CASE}/init.rc"
	expectFailure legacy_initrc_version "first init.rc command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin empty_initrc
	: > "${PATH_CASE}/init.rc"
	expectFailure empty_initrc "missing setVersion major 1" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin comments_before_initrc_version
	printf '%s\n\n' '# init.rc file description' > "${PATH_CASE}/init.rc"
	writeInitrcVersion >> "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess comments_before_initrc_version \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_minor_initrc_version
	printf '%s\n' 'setVersion major 1' > "${PATH_CASE}/init.rc"
	expectFailure missing_minor_initrc_version "missing setVersion minor 9" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin malformed_initrc_version
	printf '%s\n' 'setVersion major' > "${PATH_CASE}/init.rc"
	expectFailure malformed_initrc_version "setVersion token count" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin module_before_minor_initrc_version
	printf '%s\n' 'setVersion major 1' 'addModule service system -run core' \
		> "${PATH_CASE}/init.rc"
	expectFailure module_before_minor_initrc_version "second init.rc command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin wrong_major_initrc_version
	printf '%s\n' 'setVersion major 0' 'setVersion minor 9' \
		'addModule service must_not_be_parsed -run core -stack 256 -source_file system.c' \
		> "${PATH_CASE}/init.rc"
	expectFailure wrong_major_initrc_version "unsupported init.rc major syntax version" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logDoesNotContain wrong_major_initrc_version "found module : must_not_be_parsed"

	caseBegin wrong_minor_initrc_version
	printf '%s\n' 'setVersion major 1' 'setVersion minor 2' \
		> "${PATH_CASE}/init.rc"
	expectFailure wrong_minor_initrc_version "unsupported init.rc minor syntax version" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin wrong_initrc_version_order
	printf '%s\n' 'setVersion minor 9' 'setVersion major 1' \
		> "${PATH_CASE}/init.rc"
	expectFailure wrong_initrc_version_order "first init.rc command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin late_initrc_version
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -stack 256' 'setVersion minor 9' \
		>> "${PATH_CASE}/init.rc"
	expectFailure late_initrc_version "setVersion command after init.rc version declaration" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin malformed_initrc
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		'addModule task few' \
		'badCommand badcmd -run user -stack 256 -source_file system.c' \
		'addTask legacy -run user -stack 256 -source_file system.c' \
		'addModule task badoption -bad value -run user -stack 256 -source_file system.c' \
		'addModule task badrun -run invalid -stack 256 -source_file system.c' \
		'legacy -type user -run user -stack 256 -source_file system.c' \
		'addModule invalid badtype -run core -source_file system.c' \
		'addModule driver badi2c -run core -i2c invalid -source_file system.c' \
		'addModule task abcdefghijklmnopqrstuvwxyzabcdef -run user -stack 256 -source_file system.c' \
		'addModule task duplicate -run user -stack 256 -source_file system.c' \
		'addModule task duplicate -run user -stack 256 -source_file system.c' \
		'addModule task no_run -stack 256 -source_file system.c' \
		'addModule task run_multi -run user -run user -stack 256 -source_file system.c' \
		'addModule driver i2c_multi -i2c 1 -i2c 2 -source_file system.c' \
		'addModule task i2c_thread -run user -stack 256 -i2c 1 -source_file system.c' \
		'addModule task unterminated -run "user' >> "${PATH_CASE}/init.rc"
	VAL_INDEX=0
	while [ "${VAL_INDEX}" -le 256 ]
	do
		printf 'addModule driver driver_%03d -run core -source_file system.c\n' \
			"${VAL_INDEX}" \
			>> "${PATH_CASE}/init.rc"
		VAL_INDEX=$((VAL_INDEX + 1))
	done
	expectFailure malformed_initrc "wrong token count" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	for VAL_PATTERN in "unknown command" "unknown module type" "unknown option" "unknown data" \
		"Name too long" \
		"duplicate name" \
		"-run option is not set" "-run option is multiple set" \
		"-i2c option is multiple set" "-i2c option is not valid for task modules" \
		"too much modules" "unterminated string"
	do
		logContains malformed_initrc "${VAL_PATTERN}"
	done

	caseBegin long_initrc_line
	awk 'BEGIN { for (i = 0; i < 260; i++) printf "x"; printf "\n" }' \
		> "${PATH_CASE}/init.rc"
	expectFailure long_initrc_line "reading file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin long_initrc_list_line
	awk 'BEGIN { for (i = 0; i < 260; i++) printf "x"; printf "\n" }' \
		> "${PATH_CASE}/initrc.list"
	expectFailure long_initrc_list_line "reading file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
}

runTagCase()
{
	expectFailure "$1" "$2" "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	assertNoTemporaryFiles
}

runParseTagTests()
{
	stageBegin parse_tag
	caseBegin missing_tag_list
	sed "s|${PATH_CASE}/tags.list|${PATH_CASE}/missing.list|" \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	runTagCase missing_tag_list "opening file"

	caseBegin missing_tag_file
	printf '%s\n' "${PATH_CASE}/missing.c" > "${PATH_CASE}/tags.list"
	runTagCase missing_tag_file "opening file"

	caseBegin stop_after_tag_file_failure
	printf '%s\n' "${PATH_CASE}/missing.c" "${PATH_CASE}/must_not_be_opened.c" \
		> "${PATH_CASE}/tags.list"
	writeTags "${PATH_CASE}/must_not_be_opened.c"
	runTagCase stop_after_tag_file_failure "opening file"
	logDoesNotContain stop_after_tag_file_failure "must_not_be_opened.c"

	caseBegin malformed_tag
	printf '%s\n' '// [autoCode_tag]' > "${PATH_CASE}/tags.c"
	runTagCase malformed_tag "token count != 3"

	caseBegin nested_tag
	printf '%s\n' '// [autoCode_tag] threads_alloc' \
		'// [autoCode_tag] drivers_alloc' '// [/tag]' > "${PATH_CASE}/tags.c"
	runTagCase nested_tag "Start new tag section without previous end tag"

	caseBegin missing_end_tag
	printf '%s\n' '// [autoCode_tag] threads_alloc' > "${PATH_CASE}/tags.c"
	runTagCase missing_end_tag "missing end tag"

	caseBegin unknown_tag
	writeTags "${PATH_CASE}/tags.c"
	printf '%s\n' '// [autoCode_tag] unknown' '// [/tag]' >> "${PATH_CASE}/tags.c"
	runTagCase unknown_tag "unknown tag"

	caseBegin missing_required_tags
	printf '%s\n' 'no autoCode tags' > "${PATH_CASE}/tags.c"
	runTagCase missing_required_tags "required autoCode tag threads_alloc is not set"

	caseBegin duplicate_tag
	writeTags "${PATH_CASE}/tags.c"
	printf '%s\n' '// [autoCode_tag] error_enum' '// [/tag]' >> "${PATH_CASE}/tags.c"
	runTagCase duplicate_tag "required autoCode tag error_enum is multiple set"

	for VAL_INPUT in signals.gpio
	do
		VAL_NAME=$(printf '%s' "${VAL_INPUT}" | tr '.' '_')
		caseBegin "missing_${VAL_NAME}"
		find "${PATH_CASE}" -depth -name "${VAL_INPUT}" -delete
		runTagCase "missing_${VAL_NAME}" "opening file"
	done

	caseBegin invalid_gpio
	printf '%s\n' 'GPIO_SIGNAL_TEST extra' > "${PATH_CASE}/signals.gpio"
	runTagCase invalid_gpio "wrong token count"

	caseBegin missing_system_thread
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule task task -run user -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	runTagCase missing_system_thread "thread system was not found"

	caseBegin unterminated_tag_line
	printf '%s\n' '"unterminated' > "${PATH_CASE}/tags.c"
	for VAL_TAG in thread_stacks threads_alloc drivers_alloc thread_name_catalog \
		driver_name_catalog error_enum error_catalog modules_count modules_list gpio_signals wire_gpio \
		scli_commands
	do
		printf '%s\n%s\n' "// [autoCode_tag] ${VAL_TAG}" "// [/tag]" \
			>> "${PATH_CASE}/tags.c"
	done
	runTagCase unterminated_tag_line "unterminated string"

	for VAL_INPUT in tags.c signals.gpio
	do
		VAL_NAME=$(printf '%s' "${VAL_INPUT}" | tr '.' '_')
		caseBegin "long_${VAL_NAME}"
		awk 'BEGIN { for (i = 0; i < 260; i++) printf "x"; printf "\n" }' \
			> "${PATH_CASE}/${VAL_INPUT}"
		runTagCase "long_${VAL_NAME}" "reading file"
	done
}

runCompareReplaceTests()
{
	stageBegin compare_replace
	caseBegin stable_generation
	printf '%s\n' 'addScliCommand date' 'addScliCommand driver' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess initial_generation "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logContains initial_generation 'stack=256 words'
	if ! grep -F -q "#include \"${PATH_CASE}/sources/targetWireSignal.c\"" \
		"${PATH_CASE}/generated/wire_gpio.inc"; then
		fail "wire_gpio generated include is missing"
	fi
	if ! grep -F -q 'hal_threadContextInit(system, &(mod->context),' \
		"${PATH_CASE}/generated/threads_alloc.inc"; then
		fail "threads_alloc generated context initialization is missing"
	fi
	if ! grep -F -q 'static hal_stack_word_t thread0_stack[256];' \
		"${PATH_CASE}/generated/thread_stacks.inc"; then
		fail "thread_stacks generated static storage is missing"
	fi
	if ! grep -F -q 'mod->stack_size = 256;' \
		"${PATH_CASE}/generated/threads_alloc.inc"; then
		fail "threads_alloc generated stack size is missing"
	fi
	if ! grep -F -q \
		'&(mod->stack[mod->stack_size - MOD_STACK_CANARY_WORD_COUNT])' \
		"${PATH_CASE}/generated/threads_alloc.inc"; then
		fail "threads_alloc context does not exclude the high canary"
	fi
	if grep -F -q 'mod->stack_pointer' \
		"${PATH_CASE}/generated/threads_alloc.inc"; then
		fail "threads_alloc still initializes a stack pointer directly"
	fi
	if ! grep -F -q '#include "system/services/commands/date.h"' \
		"${PATH_CASE}/generated/scli_commands.inc"; then
		fail "scli_commands generated include is missing"
	fi
	if ! grep -F -q '{"date", dateCommand},' \
		"${PATH_CASE}/generated/scli_commands.inc"; then
		fail "scli_commands generated entry is missing"
	fi
	expectSuccess unchanged_generation "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logContains unchanged_generation "0 updated, 13 unchanged"

	sed 's/#define MOD_DRIVER_COUNT 0/#define MOD_DRIVER_COUNT 99/' \
		"${PATH_CASE}/generated/modules_count.inc" > "${PATH_CASE}/changed.inc"
	mv "${PATH_CASE}/changed.inc" "${PATH_CASE}/generated/modules_count.inc"
	expectSuccess changed_generation "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	if grep -q '#define MOD_DRIVER_COUNT 99' "${PATH_CASE}/generated/modules_count.inc"; then
		fail "changed generation did not restore generated content"
	fi
	assertNoTemporaryFiles

	caseBegin atomic_failure
	printf '%s\n' '// [autoCode_tag] threads_alloc' 'ORIGINAL_SENTINEL' '// [/tag]' \
		> "${PATH_CASE}/first.c"
	: > "${PATH_CASE}/second.c"
	for VAL_TAG in thread_stacks drivers_alloc thread_name_catalog driver_name_catalog error_enum \
		error_catalog modules_count modules_list wire_gpio scli_commands
	do
		printf '%s\n%s\n' "// [autoCode_tag] ${VAL_TAG}" "// [/tag]" \
			>> "${PATH_CASE}/second.c"
	done
	printf '%s\n' '// [autoCode_tag] gpio_signals' >> "${PATH_CASE}/second.c"
	printf '%s\n%s\n' "${PATH_CASE}/first.c" "${PATH_CASE}/second.c" \
		> "${PATH_CASE}/tags.list"
	cp "${PATH_CASE}/first.c" "${PATH_CASE}/first.expected"
	runTagCase atomic_failure "missing end tag"
	if ! cmp -s "${PATH_CASE}/first.expected" "${PATH_CASE}/first.c"; then
		fail "a failed generation modified an earlier destination"
	fi

	caseBegin output_open_failure
	chmod 0555 "${PATH_CASE}/generated"
	if "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf" \
		> "${PATH_STAGE_WORK}/output_open_failure.log" 2>&1; then
		VAL_RESULT=0
	else
		VAL_RESULT=$?
	fi
	chmod 0755 "${PATH_CASE}/generated"
	if [ "${VAL_RESULT}" -eq 0 ]; then
		fail "output_open_failure: command unexpectedly succeeded"
	fi
	logContains output_open_failure "creating file"
	if find "${PATH_CASE}/generated" -type f -print | grep -q .; then
		fail "output_open_failure: an empty destination was created"
	fi
	assertNoTemporaryFiles
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))

	caseBegin buffered_write_failure
	ln -s /dev/full "${PATH_CASE}/tags.c.tmp" || fail "cannot create /dev/full fixture"
	expectFailure buffered_write_failure "close file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	assertNoTemporaryFiles

	caseBegin remove_failure
	expectSuccess remove_failure_setup "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	cp "${PATH_CASE}/tags.c" "${PATH_CASE}/tags.c.tmp"
	chmod 0555 "${PATH_CASE}"
	if "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf" \
		> "${PATH_STAGE_WORK}/remove_failure.log" 2>&1; then
		VAL_RESULT=0
	else
		VAL_RESULT=$?
	fi
	chmod 0755 "${PATH_CASE}"
	find "${PATH_CASE}/tags.c.tmp" -type f -delete
	if [ "${VAL_RESULT}" -eq 0 ]; then
		fail "remove_failure: command unexpectedly succeeded"
	fi
	logContains remove_failure "removing temporary file"
	assertNoTemporaryFiles
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))

	caseBegin rename_failure
	expectSuccess rename_failure_setup "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	sed 's/#include "thread_stacks.inc"/#include "stale.inc"/' \
		"${PATH_CASE}/tags.c" > "${PATH_CASE}/changed.c"
	mv "${PATH_CASE}/changed.c" "${PATH_CASE}/tags.c"
	cp "${PATH_CASE}/tags.c" "${PATH_CASE}/tags.expected"
	: > "${PATH_CASE}/tags.c.tmp"
	chmod 0555 "${PATH_CASE}"
	if "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf" \
		> "${PATH_STAGE_WORK}/rename_failure.log" 2>&1; then
		VAL_RESULT=0
	else
		VAL_RESULT=$?
	fi
	chmod 0755 "${PATH_CASE}"
	find "${PATH_CASE}/tags.c.tmp" -type f -delete
	if [ "${VAL_RESULT}" -eq 0 ]; then
		fail "rename_failure: command unexpectedly succeeded"
	fi
	logContains rename_failure "renaming file"
	if ! cmp -s "${PATH_CASE}/tags.expected" "${PATH_CASE}/tags.c"; then
		fail "rename failure modified the original destination"
	fi
	assertNoTemporaryFiles
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
}

runStage()
{
	case "$1" in
		command_line) runCommandLineTests ;;
		options) runOptionTests ;;
		errors) runErrorTests ;;
		initrc) runInitrcTests ;;
		parse_tag) runParseTagTests ;;
		compare_replace) runCompareReplaceTests ;;
		*) fail "unknown stage <$1>" ;;
	esac
}

if [ ! -x "${FILE_AUTOCODE}" ]; then
	fail "autoCode executable not found: ${FILE_AUTOCODE}"
fi

if [ "${VAL_STAGE}" = all ]; then
	for VAL_SELECTED_STAGE in command_line options errors initrc parse_tag compare_replace
	do
		runStage "${VAL_SELECTED_STAGE}"
	done
else
	runStage "${VAL_STAGE}"
fi

printf 'autoCode tests passed: %s case(s)\n' "${VAL_TEST_COUNT}"
