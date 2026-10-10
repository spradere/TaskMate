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

# ------------------------------------------------------------------------------
# Description: Run one stage or the complete autoCode test corpus.
#
# Inputs: Stage name, autoCode executable path, and test work directory.
# Outputs: Test progress and diagnostics, work fixtures, and nonzero status on failure.
# ------------------------------------------------------------------------------

set -u

VAL_STAGE=$1
FILE_AUTOCODE=$2
PATH_WORK_ROOT=$3
VAL_TEST_COUNT=0
VAL_TEST_SKIP_COUNT=0

writeInitrcVersion()
{
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' 'setModuleCountMax 8' \
		'setErrorCountMax 256'
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
		driver_name_catalog driver_have error_enum error_catalog modules_count threads_list drivers_list \
		gpio_signals wire_gpio scli_commands
	do
		printf '%s\n%s\n%s\n' "// [autoCode_tag] ${VAL_TAG}" \
			"stale generated data" "// [/tag]" >> "${FILE_TAGS}"
	done
}

writeConfig()
{
	printf '%s\n' \
		"--test_mode on" \
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
	: > "${PATH_CASE}/sources/system/services/commands/scli_date.h"
	: > "${PATH_CASE}/sources/system/services/commands/scli_driver.h"
	: > "${PATH_CASE}/sources/system/services/commands/scli_date.c"
	: > "${PATH_CASE}/sources/system/services/commands/scli_driver.c"
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

logPatternCount()
{
	VAL_ACTUAL_COUNT=$(grep -F -c -- "$2" "${PATH_STAGE_WORK}/$1.log")
	if [ "${VAL_ACTUAL_COUNT}" -ne "$3" ]; then
		fail "$1: expected $3 occurrence(s) of <$2>, got ${VAL_ACTUAL_COUNT}"
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

skipTest()
{
	printf 'autoCode test skipped: %s\n' "$1"
	VAL_TEST_SKIP_COUNT=$((VAL_TEST_SKIP_COUNT + 1))
}

isCygwin()
{
	case "$(uname -s)" in
		CYGWIN*) return 0 ;;
		*) return 1 ;;
	esac
}

readOnlyDirectoryPreventsCreate()
{
	FILE_PERMISSION_PROBE="$1/.create_permission_probe"
	chmod 0555 "$1" || fail "cannot make directory read-only"
	if ( : > "${FILE_PERMISSION_PROBE}" ) 2> /dev/null; then
		VAL_PERMISSION_DENIED=0
	else
		VAL_PERMISSION_DENIED=1
	fi
	chmod 0755 "$1" || fail "cannot restore directory permissions"
	rm -f "${FILE_PERMISSION_PROBE}"
	[ "${VAL_PERMISSION_DENIED}" -eq 1 ]
}

readOnlyDirectoryPreventsRemove()
{
	FILE_PERMISSION_PROBE="$1/.remove_permission_probe"
	: > "${FILE_PERMISSION_PROBE}" || fail "cannot create remove permission probe"
	chmod 0555 "$1" || fail "cannot make directory read-only"
	if rm -f "${FILE_PERMISSION_PROBE}" 2> /dev/null; then
		VAL_PERMISSION_DENIED=0
	else
		VAL_PERMISSION_DENIED=1
	fi
	chmod 0755 "$1" || fail "cannot restore directory permissions"
	rm -f "${FILE_PERMISSION_PROBE}"
	[ "${VAL_PERMISSION_DENIED}" -eq 1 ]
}

readOnlyDirectoryPreventsRename()
{
	FILE_PERMISSION_PROBE="$1/.rename_permission_probe"
	FILE_PERMISSION_RENAMED="$1/.renamed_permission_probe"
	: > "${FILE_PERMISSION_PROBE}" || fail "cannot create rename permission probe"
	chmod 0555 "$1" || fail "cannot make directory read-only"
	if mv "${FILE_PERMISSION_PROBE}" "${FILE_PERMISSION_RENAMED}" 2> /dev/null; then
		VAL_PERMISSION_DENIED=0
	else
		VAL_PERMISSION_DENIED=1
	fi
	chmod 0755 "$1" || fail "cannot restore directory permissions"
	rm -f "${FILE_PERMISSION_PROBE}" "${FILE_PERMISSION_RENAMED}"
	[ "${VAL_PERMISSION_DENIED}" -eq 1 ]
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
	printf '%s\n' '--test_mode' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure wrong_token_count "wrong token count"

	caseBegin unknown_option
	printf '%s\n' '--unknown value' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure unknown_option "unknown option"

	caseBegin all_required_missing
	: > "${PATH_CASE}/autoCode.conf"
	runOptionFailure all_required_missing \
		"required autoCode option --test_mode is not set"

	caseBegin all_required_duplicate
	cp "${PATH_CASE}/autoCode.conf" "${PATH_CASE}/duplicate.conf"
	cat "${PATH_CASE}/duplicate.conf" >> "${PATH_CASE}/autoCode.conf"
	runOptionFailure all_required_duplicate \
		"required autoCode option --test_mode is multiple set"

	for VAL_VALUE in yes ON 1 0
	do
		VAL_NAME=$(printf '%s' "${VAL_VALUE}" | tr -c '[:alnum:]' '_')
		caseBegin "invalid_test_mode_${VAL_NAME}"
		sed "s/--test_mode on/--test_mode ${VAL_VALUE}/" \
			"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
		mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
		runOptionFailure "invalid_test_mode_${VAL_NAME}" "invalid --test_mode value"
	done

	caseBegin invalid_source_path
	sed "s|--source_path .*|--source_path ${PATH_CASE}/sources/system.c|" \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	runOptionFailure invalid_source_path "invalid --source_path directory"

	caseBegin unterminated_option
	printf '%s\n' '--test_mode "on' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure unterminated_option "unterminated string"
	logContains unterminated_option "autoCode.conf:1"

	caseBegin long_option_line
	awk 'BEGIN { printf "--errors "; for (i = 0; i < 260; i++) printf "x"; \
		printf "\n" }' > "${PATH_CASE}/autoCode.conf"
	runOptionFailure long_option_line "reading file"
}

runErrorTests()
{
	stageBegin errors
	caseBegin normal_mode_error_limit
	sed 's/--test_mode on/--test_mode off/' \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	printf '%s\n' 'ERR_FIRST' 'ERR_SECOND' > "${PATH_CASE}/errors.err"
	expectFailure normal_mode_error_limit "ERR_FIRST" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logPatternCount normal_mode_error_limit "wrong token count != 3" 1
	logDoesNotContain normal_mode_error_limit "ERR_SECOND"

	caseBegin test_mode_error_limit
	: > "${PATH_CASE}/errors.err"
	VAL_INDEX=0
	while [ "${VAL_INDEX}" -le 100 ]
	do
		printf 'ERR_TEST_LIMIT_%03d\n' "${VAL_INDEX}" >> "${PATH_CASE}/errors.err"
		VAL_INDEX=$((VAL_INDEX + 1))
	done
	expectFailure test_mode_error_limit "ERR_TEST_LIMIT_000" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logPatternCount test_mode_error_limit "wrong token count != 3" 100
	logDoesNotContain test_mode_error_limit "ERR_TEST_LIMIT_100"

	caseBegin missing_error_list
	sed "s|${PATH_CASE}/errors.list|${PATH_CASE}/missing.list|" \
		"${PATH_CASE}/autoCode.conf" > "${PATH_CASE}/changed.conf"
	mv "${PATH_CASE}/changed.conf" "${PATH_CASE}/autoCode.conf"
	expectFailure missing_error_list "opening file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logContains missing_error_list "${PATH_CASE}/missing.list"

	caseBegin error_list_before_initrc_list
	find "${PATH_CASE}/errors.list" "${PATH_CASE}/initrc.list" -delete
	expectFailure error_list_before_initrc_list "${PATH_CASE}/errors.list" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logDoesNotContain error_list_before_initrc_list "opening file <${PATH_CASE}/initrc.list>"

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
	logContains malformed_errors "errors.err:6"

	caseBegin unterminated_error_list
	printf '%s\n' '"unterminated' "${PATH_CASE}/errors.err" \
		> "${PATH_CASE}/errors.list"
	expectFailure unterminated_error_list "unterminated string" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logContains unterminated_error_list "errors.list:1"
	logContains unterminated_error_list "open file.err <${PATH_CASE}/errors.err>"

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

	for VAL_LIMIT in 255 256
	do
		caseBegin "error_count_${VAL_LIMIT}"
		if [ "${VAL_LIMIT}" -eq 255 ]; then
			printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
				'setModuleCountMax 8' 'setErrorCountMax 255' \
				'addModule service system -run core -stack 256 -source_file system.c' \
				> "${PATH_CASE}/init.rc"
		fi
		: > "${PATH_CASE}/errors.err"
		VAL_INDEX=0
		while [ "${VAL_INDEX}" -lt "${VAL_LIMIT}" ]
		do
			if [ "${VAL_INDEX}" -eq 0 ] || [ "${VAL_INDEX}" -eq $((VAL_LIMIT - 1)) ]; then
				printf 'ERR_BOUND_%03d "boundary" WARN\n' "${VAL_INDEX}" \
					>> "${PATH_CASE}/errors.err"
			else
				printf 'ERR_BOUND_%03d "" FLOW\n' "${VAL_INDEX}" \
					>> "${PATH_CASE}/errors.err"
			fi
			VAL_INDEX=$((VAL_INDEX + 1))
		done
		expectSuccess "error_count_${VAL_LIMIT}" \
			"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
		if [ "${VAL_LIMIT}" -eq 255 ]; then
			VAL_WIDTH=8
			VAL_SIZE=1
		else
			VAL_WIDTH=16
			VAL_SIZE=2
		fi
		if ! grep -F -q "typedef uint${VAL_WIDTH}_t error_count_t;" \
			"${PATH_CASE}/generated/error_enum.inc"; then
			fail "error_count_${VAL_LIMIT}: wrong count type"
		fi
		if ! printf '%s\n' '#include <stdint.h>' '#include "error_enum.inc"' \
			"_Static_assert(sizeof(err_codes_t) == ${VAL_SIZE}U, \"wrong code size\");" | \
			clang -std=gnu11 -I"${PATH_CASE}/generated" -x c -fsyntax-only -; then
			fail "error_count_${VAL_LIMIT}: generated enum size is invalid"
		fi
		if ! clang -std=gnu11 -Wall -Wextra -Wconversion -Wenum-conversion \
			-I"srcs" -I"${PATH_CASE}/generated" \
			"test/sysCall/test_sc_errors.c" -o "${PATH_CASE}/test_sc_errors"; then
			fail "error_count_${VAL_LIMIT}: error lookup test did not compile"
		fi
		if ! "${PATH_CASE}/test_sc_errors"; then
			fail "error_count_${VAL_LIMIT}: error lookup failed"
		fi
	done
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
	logContains missing_initrc_list "${PATH_CASE}/missing.list"

	caseBegin initrc_list_before_tag_list
	find "${PATH_CASE}/initrc.list" "${PATH_CASE}/tags.list" -delete
	expectFailure initrc_list_before_tag_list "${PATH_CASE}/initrc.list" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logDoesNotContain initrc_list_before_tag_list "opening file <${PATH_CASE}/tags.list>"

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

	caseBegin default_stack_size_min
	printf '%s\n' 'addModule task task -run user -stack 3 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess default_stack_size_min "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin configured_stack_size_min
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setStackSizeMin 64' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		'addModule task task -run user -stack 64 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess configured_stack_size_min "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin below_stack_size_min
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setStackSizeMin 64' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		'addModule task task -run user -stack 63 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure below_stack_size_min '63 for option -stack' \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin configured_large_stack_size_min
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setStackSizeMin 8192' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		'addModule task task -run user -stack 8192 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess configured_large_stack_size_min \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	for VAL_SIZE in 0 2 65536 invalid -1 999999999999999999999999999999
	do
		caseBegin "invalid_stack_size_min_${VAL_SIZE}"
		writeInitrcVersion > "${PATH_CASE}/init.rc"
		printf '%s\n' "setStackSizeMin ${VAL_SIZE}" >> "${PATH_CASE}/init.rc"
		expectFailure "invalid_stack_size_min_${VAL_SIZE}" \
			'invalid minimum stack size' "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	done

	caseBegin malformed_stack_size_min
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setStackSizeMin 64 extra' >> "${PATH_CASE}/init.rc"
	expectFailure malformed_stack_size_min 'setStackSizeMin token count' \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin duplicate_stack_size_min
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setStackSizeMin 64' 'setStackSizeMin 8192' \
		>> "${PATH_CASE}/init.rc"
	expectFailure duplicate_stack_size_min 'setStackSizeMin is already defined' \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin late_stack_size_min
	printf '%s\n' 'setStackSizeMin 64' >> "${PATH_CASE}/init.rc"
	expectFailure late_stack_size_min 'setStackSizeMin must be defined before addModule' \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin valid_commands
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' \
		'addModule driver driver -run driver -i2c 0x20 -source_file system.c' \
		'addModule service system -run service -stack 256 -source_file system.c' \
		'addModule task task -run user -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_commands "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	if ! grep -F -q 'typedef uint8_t mod_count_t;' \
		"${PATH_CASE}/generated/modules_count.inc"; then
		fail "valid_commands: generated module count type is missing"
	fi
	if ! grep -F -q '#define MOD_DRIVER_COUNT 1U' \
		"${PATH_CASE}/generated/modules_count.inc" || \
		! grep -F -q '#define MOD_THREAD_COUNT 2U' \
		"${PATH_CASE}/generated/modules_count.inc"; then
		fail "valid_commands: module categories are incorrect"
	fi
	if ! grep -F -q 'control_data.run_level = RL_GET_RUN_LEVEL(2U);' \
		"${PATH_CASE}/generated/drivers_alloc.inc" || \
		! grep -F -q 'mod->saved_run_level = RL_GET_RUN_LEVEL(3U);' \
		"${PATH_CASE}/generated/threads_alloc.inc" || \
		! grep -F -q 'mod->saved_run_level = RL_GET_RUN_LEVEL(4U);' \
		"${PATH_CASE}/generated/threads_alloc.inc"; then
		fail "valid_commands: generated run levels are incorrect"
	fi
	if ! grep -F -q 'typedef uint16_t error_count_t;' \
		"${PATH_CASE}/generated/error_enum.inc"; then
		fail "valid_commands: generated error count type is missing"
	fi

	caseBegin missing_error_count
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
		'setModuleCountMax 8' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		> "${PATH_CASE}/init.rc"
	expectFailure missing_error_count "missing setErrorCountMax" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	for VAL_COUNT in 0 257 invalid -1
	do
		VAL_NAME=$(printf '%s' "${VAL_COUNT}" | tr -c '[:alnum:]' '_')
		caseBegin "invalid_error_count_${VAL_NAME}"
		printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
			'setModuleCountMax 8' "setErrorCountMax ${VAL_COUNT}" \
			> "${PATH_CASE}/init.rc"
		expectFailure "invalid_error_count_${VAL_NAME}" "expected 1..256" \
			"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	done

	caseBegin duplicate_error_count
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setErrorCountMax 256' >> "${PATH_CASE}/init.rc"
	expectFailure duplicate_error_count "setErrorCountMax is already defined" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin malformed_error_count
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
		'setModuleCountMax 8' 'setErrorCountMax 8 extra' \
		> "${PATH_CASE}/init.rc"
	expectFailure malformed_error_count "setErrorCountMax token count" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin error_count_overflow
	printf '%s\n' 'ERR_FIRST "" FLOW' 'ERR_SECOND "" FLOW' \
		> "${PATH_CASE}/errors.err"
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
		'setModuleCountMax 8' 'setErrorCountMax 1' \
		> "${PATH_CASE}/init.rc"
	expectFailure error_count_overflow "error count exceeds configured maximum 1" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin old_module_count_command
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
		'setModuleCount 8' 'setErrorCountMax 256' \
		> "${PATH_CASE}/init.rc"
	expectFailure old_module_count_command "unknown command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin valid_16_bit_module_count
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' 'setModuleCountMax 256' \
		'setErrorCountMax 256' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		> "${PATH_CASE}/init.rc"
	expectSuccess valid_16_bit_module_count \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	if ! grep -F -q 'typedef uint16_t mod_count_t;' \
		"${PATH_CASE}/generated/modules_count.inc"; then
		fail "valid_16_bit_module_count: generated module count type is missing"
	fi

	caseBegin missing_module_count
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
		'addModule service system -run core -stack 256 -source_file system.c' \
		> "${PATH_CASE}/init.rc"
	expectFailure missing_module_count "setModuleCountMax must be defined before addModule" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	for VAL_COUNT in 0 65536 invalid -1
	do
		VAL_NAME=$(printf '%s' "${VAL_COUNT}" | tr -c '[:alnum:]' '_')
		caseBegin "invalid_module_count_${VAL_NAME}"
		printf '%s\n' 'setVersion major 1' 'setVersion minor 12' \
			"setModuleCountMax ${VAL_COUNT}" > "${PATH_CASE}/init.rc"
		expectFailure "invalid_module_count_${VAL_NAME}" "expected 1..65535" \
			"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	done

	caseBegin malformed_module_count
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' 'setModuleCountMax 8 extra' \
		> "${PATH_CASE}/init.rc"
	expectFailure malformed_module_count "setModuleCountMax token count" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin duplicate_module_count
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'setModuleCountMax 8' >> "${PATH_CASE}/init.rc"
	expectFailure duplicate_module_count "setModuleCountMax is already defined" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin module_count_boundary
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' 'setModuleCountMax 255' \
		'setErrorCountMax 256' \
		> "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	VAL_INDEX=0
	while [ "${VAL_INDEX}" -lt 254 ]
	do
		printf 'addModule driver driver_%03d -run core -source_file system.c\n' \
			"${VAL_INDEX}" >> "${PATH_CASE}/init.rc"
		VAL_INDEX=$((VAL_INDEX + 1))
	done
	expectSuccess module_count_boundary "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin module_count_overflow
	printf '%s\n' 'setVersion major 1' 'setVersion minor 12' 'setModuleCountMax 255' \
		'setErrorCountMax 256' \
		> "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -stack 256 -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	VAL_INDEX=0
	while [ "${VAL_INDEX}" -lt 255 ]
	do
		printf 'addModule driver driver_%03d -run core -source_file system.c\n' \
			"${VAL_INDEX}" >> "${PATH_CASE}/init.rc"
		VAL_INDEX=$((VAL_INDEX + 1))
	done
	expectFailure module_count_overflow "module count exceeds configured maximum 255" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

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
	printf '%s\n' \
		'addScliCommand date -source_file system/services/commands/scli_date.c' \
		'addScliCommand driver -source_file system/services/commands/scli_driver.c' \
		>> "${PATH_CASE}/init.rc"
	expectSuccess valid_scli_commands "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_scli_source_file
	printf '%s\n' 'addScliCommand date' >> "${PATH_CASE}/init.rc"
	expectFailure missing_scli_source_file "addScliCommand token count" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin invalid_scli_source_option
	printf '%s\n' 'addScliCommand date -source_dir system/services/commands' \
		>> "${PATH_CASE}/init.rc"
	expectFailure invalid_scli_source_option "addScliCommand unknown option" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_scli_source_file_path
	printf '%s\n' 'addScliCommand date -source_file system/services/commands/missing.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure missing_scli_source_file_path "SCLI command source file not found" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin unknown_scli_command
	printf '%s\n' \
		'addScliCommand missing -source_file system/services/commands/scli_date.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure unknown_scli_command "unknown SCLI command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin missing_scli_command_header
	find "${PATH_CASE}/sources/system/services/commands/scli_date.h" -delete
	printf '%s\n' \
		'addScliCommand date -source_file system/services/commands/scli_date.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure missing_scli_command_header "SCLI command header not found" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin long_scli_command_identifier
	printf '%s\n' \
		'addScliCommand abcdefghijklmnopqrstuvwxyzabcdef -source_file system.c' \
		>> "${PATH_CASE}/init.rc"
	expectFailure long_scli_command_identifier "SCLI command identifier is too long" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin duplicate_scli_command
	printf '%s\n' \
		'addScliCommand date -source_file system/services/commands/scli_date.c' \
		'addScliCommand date -source_file system/services/commands/scli_date.c' \
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
	expectFailure missing_minor_initrc_version "missing setVersion minor 12" \
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
	printf '%s\n' 'setVersion major 0' 'setVersion minor 12' \
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
	printf '%s\n' 'setVersion minor 12' 'setVersion major 1' \
		> "${PATH_CASE}/init.rc"
	expectFailure wrong_initrc_version_order "first init.rc command" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"

	caseBegin late_initrc_version
	writeInitrcVersion > "${PATH_CASE}/init.rc"
	printf '%s\n' 'addModule service system -run core -stack 256' 'setVersion minor 12' \
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
		"module count exceeds configured maximum" "unterminated string"
	do
		logContains malformed_initrc "${VAL_PATTERN}"
	done
	logContains malformed_initrc "init.rc:21"

	caseBegin unterminated_initrc_list
	printf '%s\n' '"unterminated' "${PATH_CASE}/init.rc" \
		> "${PATH_CASE}/initrc.list"
	expectFailure unterminated_initrc_list "unterminated string" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logContains unterminated_initrc_list "initrc.list:1"
	logContains unterminated_initrc_list "open <${PATH_CASE}/init.rc>"

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
		driver_name_catalog driver_have error_enum error_catalog modules_count threads_list drivers_list \
		gpio_signals wire_gpio scli_commands
	do
		printf '%s\n%s\n' "// [autoCode_tag] ${VAL_TAG}" "// [/tag]" \
			>> "${PATH_CASE}/tags.c"
	done
	runTagCase unterminated_tag_line "unterminated string"
	logContains unterminated_tag_line "tags.c:1"

	caseBegin unterminated_tag_list
	printf '%s\n' '"unterminated' "${PATH_CASE}/tags.c" \
		> "${PATH_CASE}/tags.list"
	runTagCase unterminated_tag_list "unterminated string"
	logContains unterminated_tag_list "tags.list:1"
	logContains unterminated_tag_list "open <${PATH_CASE}/tags.c>"

	caseBegin unterminated_gpio
	printf '%s\n' '"unterminated' 'GPIO_SIGNAL_TEST' \
		> "${PATH_CASE}/signals.gpio"
	runTagCase unterminated_gpio "unterminated string"
	logContains unterminated_gpio "signals.gpio:1"

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
	printf '%s\n' \
		'addModule driver timerContext -run driver -source_file system.c' \
		'addScliCommand date -source_file system/services/commands/scli_date.c' \
		'addScliCommand driver -source_file system/services/commands/scli_driver.c' \
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
	if ! grep -F -q '#include "system/services/system.h"' \
		"${PATH_CASE}/generated/threads_list.inc"; then
		fail "threads_list generated service declaration is missing"
	fi
	if ! grep -F -q '#include "interfaces/drv_timerContext.h"' \
		"${PATH_CASE}/generated/drivers_list.inc"; then
		fail "drivers_list generated driver declaration is missing"
	fi
	if ! grep -F -q '#define TM_DRIVER_HAVE_TIMER_CONTEXT 1U' \
		"${PATH_CASE}/generated/driver_have.inc"; then
		fail "driver_have generated presence definition is missing"
	fi
	if ! grep -F -q 'mod->software_time_counter = 0U;' \
		"${PATH_CASE}/generated/threads_alloc.inc"; then
		fail "threads_alloc generated zero initialization is missing"
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
	if ! grep -F -q '#include "system/services/commands/scli_date.h"' \
		"${PATH_CASE}/generated/scli_commands.inc"; then
		fail "scli_commands generated include is missing"
	fi
	if ! grep -F -q '{"date", dateCommand},' \
		"${PATH_CASE}/generated/scli_commands.inc"; then
		fail "scli_commands generated entry is missing"
	fi
	if ! grep -F -q '{0U, 0U},' \
		"${PATH_CASE}/generated/scli_commands.inc"; then
		fail "scli_commands generated sentinel is missing"
	fi
	expectSuccess unchanged_generation "${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	logContains unchanged_generation "0 updated, 15 unchanged"

	sed 's/#define MOD_DRIVER_COUNT 1/#define MOD_DRIVER_COUNT 99/' \
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
	for VAL_TAG in thread_stacks drivers_alloc thread_name_catalog driver_name_catalog \
		driver_have error_enum \
		error_catalog modules_count threads_list drivers_list wire_gpio scli_commands
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
	if readOnlyDirectoryPreventsCreate "${PATH_CASE}/generated"; then
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
		logContains output_open_failure "creating temporary file"
		if find "${PATH_CASE}/generated" -type f -print | grep -q .; then
			fail "output_open_failure: an empty destination was created"
		fi
		assertNoTemporaryFiles
		VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
	else
		skipTest "output_open_failure: read-only directory permits file creation"
	fi

	caseBegin buffered_write_failure
	ln -s /dev/full "${PATH_CASE}/tags.c.tmp" || fail "cannot create /dev/full fixture"
	expectFailure buffered_write_failure "closing temporary file" \
		"${FILE_AUTOCODE}" "${PATH_CASE}/autoCode.conf"
	assertNoTemporaryFiles

	caseBegin remove_failure
	if isCygwin; then
		skipTest "remove_failure: Cygwin does not enforce POSIX remove permissions"
	elif readOnlyDirectoryPreventsRemove "${PATH_CASE}"; then
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
	else
		skipTest "remove_failure: read-only directory permits file removal"
	fi

	caseBegin rename_failure
	if isCygwin; then
		skipTest "rename_failure: Cygwin does not enforce POSIX rename permissions"
	elif readOnlyDirectoryPreventsRename "${PATH_CASE}"; then
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
		logContains rename_failure "renaming temporary file failed"
		logContains rename_failure "[autoCode.c:"
		if ! cmp -s "${PATH_CASE}/tags.expected" "${PATH_CASE}/tags.c"; then
			fail "rename failure modified the original destination"
		fi
		assertNoTemporaryFiles
		VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
	else
		skipTest "rename_failure: read-only directory permits file rename"
	fi
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
if [ "${VAL_TEST_SKIP_COUNT}" -ne 0 ]; then
	printf 'autoCode tests skipped: %s case(s)\n' "${VAL_TEST_SKIP_COUNT}"
fi
