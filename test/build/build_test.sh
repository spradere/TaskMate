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
PATH_PROJECT=$(realpath "$2")
PATH_WORK_ROOT=$3
VAL_TEST_COUNT=0

fail()
{
	printf 'build-system test failure: %s\n' "$1" >&2
	exit 1
}

stageBegin()
{
	PATH_STAGE_WORK="${PATH_WORK_ROOT}/$1"
	if [ -d "${PATH_STAGE_WORK}" ]; then
		find "${PATH_STAGE_WORK}" -depth -delete || fail "cannot clean ${PATH_STAGE_WORK}"
	fi
	mkdir -p "${PATH_STAGE_WORK}" || fail "cannot create ${PATH_STAGE_WORK}"
	PATH_STAGE_WORK=$(realpath "${PATH_STAGE_WORK}")
	printf '\nbuild-system test stage: %s\n' "$1"
}

logContains()
{
	if ! grep -F -q -- "$2" "${PATH_STAGE_WORK}/$1.log"; then
		fail "$1: missing output <$2>"
	fi
}

logExcludes()
{
	if grep -F -q -- "$2" "${PATH_STAGE_WORK}/$1.log"; then
		fail "$1: unexpected output <$2>"
	fi
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

expectOutput()
{
	VAL_NAME=$1
	VAL_EXPECTED=$2
	shift 2
	expectSuccess "${VAL_NAME}" "$@"
	VAL_ACTUAL=$(cat "${PATH_STAGE_WORK}/${VAL_NAME}.log")
	if [ "${VAL_ACTUAL}" != "${VAL_EXPECTED}" ]; then
		fail "${VAL_NAME}: expected <${VAL_EXPECTED}>, got <${VAL_ACTUAL}>"
	fi
}

assertFileContains()
{
	if ! grep -F -q -- "$2" "$1"; then
		fail "$1: missing content <$2>"
	fi
}

assertFileExcludes()
{
	if grep -F -q -- "$2" "$1"; then
		fail "$1: unexpected content <$2>"
	fi
}

runConfigurationTests()
{
	stageBegin configuration

	expectOutput default_stack "test1 arduinoMega atmega2560 avr8" \
		bmake -C "${PATH_PROJECT}" -V VAL_HW_STACK
	assertFileExcludes "${PATH_PROJECT}/conf/hardware-targets.conf" "test_noscli"
	expectOutput default_path "build/test1_arduinoMega_atmega2560_avr8" \
		bmake -C "${PATH_PROJECT}" -V PATH_BUILD_TARGET
	expectOutput architecture_compiler "srcs/hal/arch/avr8/avr8_CC.mk" \
		bmake -C "${PATH_PROJECT}" -V FILE_ARCH_CC
	expectOutput architecture_types_header \
		"srcs/hal/arch/avr8/avr8_architecture_types.h" \
		bmake -C "${PATH_PROJECT}" -V FILE_HAL_ARCHITECTURE_TYPES
	expectSuccess architecture_types_compile_flag bmake -C "${PATH_PROJECT}" -V CFLAGS
	logContains architecture_types_compile_flag \
		"-include srcs/hal/arch/avr8/avr8_architecture_types.h"
	expectSuccess source_search_paths bmake -C "${PATH_PROJECT}" -V PATHS_SOURCE_SEARCH
	logExcludes source_search_paths "srcs/hal/public"
	expectOutput cpu_frequency "16000000UL" bmake -C "${PATH_PROJECT}" -V VAL_CPU_FREQ
	expectOutput mcu_serial "atmega2560" bmake -C "${PATH_PROJECT}" -V VAL_MCU_SERIAL

	expectSuccess default_compile_sources bmake -C "${PATH_PROJECT}" -V FILES_COMPILE_SRC
	logContains default_compile_sources "srcs/hal/arch/avr8/avr8_atomic.c"
	logContains default_compile_sources "srcs/system/services/commands/date.c"
	logContains default_compile_sources "srcs/system/services/commands/stack.c"

	expectSuccess default_initrc_sources bmake -C "${PATH_PROJECT}" -V FILES_INITRC_SRC
	logContains default_initrc_sources "srcs/system/services/scli.c"
	logContains default_initrc_sources "srcs/hal/mcu/atmega2560/at2560_timerSched.c"
	expectOutput default_initrc_dirs "srcs/system/services/commands" \
		bmake -C "${PATH_PROJECT}" -V PATHS_INITRC_SOURCES

	expectSuccess default_extra_sources bmake -C "${PATH_PROJECT}" -V FILES_EXTRA_SRC
	logContains default_extra_sources "srcs/hal/mcu/atmega2560/at2560_gpio.c"
	logExcludes default_extra_sources "srcs/user/target/test1/targetWireSignal.c"

	expectSuccess initrc_directory_sources bmake -C "${PATH_PROJECT}" -V FILES_INITRC_DIR_SRC
	logContains initrc_directory_sources "srcs/system/services/commands/date.c"
	logExcludes initrc_directory_sources "srcs/hal/arch/avr8/avr8_context.c"

	expectSuccess object_mapping bmake -C "${PATH_PROJECT}" -V FILES_OBJ
	logContains object_mapping \
		"build/test1_arduinoMega_atmega2560_avr8/srcs/system/boot.o"
	logExcludes object_mapping ".c.o"
	VAL_DRIVER_INTERFACES="srcs/interfaces/drv_i2c.h srcs/interfaces/drv_lcd.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_rtc.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_timerSTC.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_timerSched.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_usart.h"
	expectOutput driver_interface_order "${VAL_DRIVER_INTERFACES}" \
		bmake -C "${PATH_PROJECT}" -V FILES_DRIVER_INTERFACES

	expectFailure invalid_target "Target not found >>>missing/target.mk<<<" \
		bmake -C "${PATH_PROJECT}" VAL_TARGET=missing -V VAL_HW_STACK
	expectFailure invalid_option 'Invalid option "invalid"' \
		bmake -C "${PATH_PROJECT}" OPT_CLEAN_AUTOCODE_LOGS=invalid -V VAL_HW_STACK

	PATH_MANIFESTS="${PATH_STAGE_WORK}/manifests"
	: > "${PATH_STAGE_WORK}/programs.conf"
	expectSuccess dependency_manifests bmake -C "${PATH_PROJECT}" \
		PATH_BUILD_TARGET="${PATH_MANIFESTS}" \
		FILE_TM_INFO="${PATH_STAGE_WORK}/tm_info.h" \
		FILE_BUILD_INFO="${PATH_STAGE_WORK}/build_info.txt" \
		CONF_PROGRAMS_LIST="${PATH_STAGE_WORK}/programs.conf" \
		FILE_PROGRAMS_CHECK_STAMP="${PATH_STAGE_WORK}/programs.stamp" \
		FILE_AVR8_PROGRAMS_LIST="${PATH_STAGE_WORK}/programs.conf" \
		FILE_AVR8_PROGRAMS_CHECK_STAMP="${PATH_STAGE_WORK}/avr8_programs.stamp" \
		_autocode_dependency_check
	assertFileContains "${PATH_MANIFESTS}/gpio_signals.deps" \
		"srcs/user/target/test1/signals.gpio"
	expectFailure wire_gpio_outside "Path outside current directory rejected" \
		bmake -C "${PATH_PROJECT}" \
		FILE_WIREGPIO="/etc/passwd" \
		_autocode_dependency_check
	expectFailure wire_gpio_wrong_type "Invalid -f path rejected" \
		bmake -C "${PATH_PROJECT}" FILE_WIREGPIO="${PATH_PROJECT}/build" \
		_autocode_dependency_check

	PATH_DEPENDENCIES="${PATH_STAGE_WORK}/dependencies"
	mkdir -p "${PATH_DEPENDENCIES}"
	printf 'fixture.o: fixture.c \\\r\n fixture.h\r\n' > "${PATH_DEPENDENCIES}/fixture.d"
	expectSuccess dependencies_crlf_first bmake -C "${PATH_PROJECT}" \
		PATH_BUILD_TARGET="${PATH_DEPENDENCIES}" \
		FILES_DEP="${PATH_DEPENDENCIES}/fixture.d" \
		FILE_DEPS_ALL="${PATH_DEPENDENCIES}/.deps.d" _dependency
	expectSuccess dependencies_crlf_second bmake -C "${PATH_PROJECT}" \
		PATH_BUILD_TARGET="${PATH_DEPENDENCIES}" \
		FILES_DEP="${PATH_DEPENDENCIES}/fixture.d" \
		FILE_DEPS_ALL="${PATH_DEPENDENCIES}/.deps.d" _dependency
	if LC_ALL=C grep -q "$(printf '\r')" "${PATH_DEPENDENCIES}/.deps.d"; then
		fail "dependency aggregation retained CR characters"
	fi

	printf 'build-system configuration tests passed: %d cases\n' "${VAL_TEST_COUNT}"
}

runScriptTests()
{
	stageBegin scripts
	FILE_PROGRAMS="${PATH_PROJECT}/scripts/check_programs.sh"
	FILE_COMPARE="${PATH_PROJECT}/scripts/compare_replace.sh"
	FILE_PATH_CHECK="${PATH_PROJECT}/scripts/check_path_file.sh"
	FILE_VERSION="${PATH_PROJECT}/scripts/git_version.sh"
	FILE_AUTOCODE_VERSION="${PATH_PROJECT}/scripts/autocode_version.awk"
	FILE_HAL_FACADE="${PATH_PROJECT}/scripts/check_removed_hal_facade.sh"

	printf '%s\n' '#define AC_INITRC_EXPECTED_VER_MAJOR 1' \
		'#define AC_INITRC_EXPECTED_VER_MINOR 5' \
		'#define AC_AUTOCODE_VER_MAJOR 1' \
		'#define AC_AUTOCODE_VER_MINOR 2' > "${PATH_STAGE_WORK}/autoCode.h"
	expectSuccess autocode_version_match awk -v expected_major=1 -v expected_minor=2 \
		-f "${FILE_AUTOCODE_VERSION}" "${PATH_STAGE_WORK}/autoCode.h"
	expectOutput autocode_version_report "autoCode : 1.2
initrc : 1.5" awk -v expected_major=1 -v expected_minor=2 -v report_versions=1 \
		-f "${FILE_AUTOCODE_VERSION}" "${PATH_STAGE_WORK}/autoCode.h"
	expectFailure autocode_version_major_mismatch "expected 2, found 1" awk \
		-v expected_major=2 -v expected_minor=2 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/autoCode.h"
	expectFailure autocode_version_minor_mismatch "expected 3, found 2" awk \
		-v expected_major=1 -v expected_minor=3 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/autoCode.h"
	printf '%s\n' '#define AC_AUTOCODE_VER_MAJOR 1' > "${PATH_STAGE_WORK}/missing.h"
	expectFailure autocode_version_missing "Missing autoCode version definition" awk \
		-v expected_major=1 -v expected_minor=2 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/missing.h"
	printf '%s\n' '#define AC_AUTOCODE_VER_MAJOR one' \
		'#define AC_AUTOCODE_VER_MINOR 2' > "${PATH_STAGE_WORK}/malformed.h"
	expectFailure autocode_version_malformed "Invalid autoCode version definition" awk \
		-v expected_major=1 -v expected_minor=2 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/malformed.h"

	expectFailure programs_usage "Usage:" "${FILE_PROGRAMS}"
	expectFailure programs_missing_file "Programs list is not readable" \
		"${FILE_PROGRAMS}" "${PATH_STAGE_WORK}/missing.conf"
	printf '%s\n' '# ignored' '' sh awk > "${PATH_STAGE_WORK}/available.conf"
	expectSuccess programs_available "${FILE_PROGRAMS}" "${PATH_STAGE_WORK}/available.conf"
	printf '%s' sh > "${PATH_STAGE_WORK}/no_newline.conf"
	expectSuccess programs_no_final_newline \
		"${FILE_PROGRAMS}" "${PATH_STAGE_WORK}/no_newline.conf"
	printf '%s\n' sh taskmate_program_that_does_not_exist > "${PATH_STAGE_WORK}/missing.conf"
	expectFailure programs_missing "Missing required programs:" \
		"${FILE_PROGRAMS}" "${PATH_STAGE_WORK}/missing.conf"
	logContains programs_missing "taskmate_program_that_does_not_exist"

	PATH_HAL_REMOVAL="${PATH_STAGE_WORK}/hal_removal_project"
	mkdir -p "${PATH_HAL_REMOVAL}/srcs" "${PATH_HAL_REMOVAL}/mk" \
		"${PATH_HAL_REMOVAL}/conf" "${PATH_HAL_REMOVAL}/test"
	: > "${PATH_HAL_REMOVAL}/Makefile"
	printf '%s\n' 'hal/public is rejected by the guard' \
		> "${PATH_HAL_REMOVAL}/test/negative.c"
	expectSuccess hal_removal_clean "${FILE_HAL_FACADE}" "${PATH_HAL_REMOVAL}"

	mkdir -p "${PATH_HAL_REMOVAL}/srcs/hal/public"
	expectFailure hal_removal_directory "Removed HAL public directory still exists" \
		"${FILE_HAL_FACADE}" "${PATH_HAL_REMOVAL}"
	rmdir "${PATH_HAL_REMOVAL}/srcs/hal/public"

	printf '%s\n' '#include "hal/public/hal_context.h"' \
		> "${PATH_HAL_REMOVAL}/srcs/legacy.c"
	expectFailure hal_removal_token "Legacy HAL public token" \
		"${FILE_HAL_FACADE}" "${PATH_HAL_REMOVAL}"
	find "${PATH_HAL_REMOVAL}/srcs/legacy.c" -delete

	printf '%s\n' '#define HAL_CONTEXT 1' > "${PATH_HAL_REMOVAL}/srcs/legacy.c"
	expectFailure hal_removal_macro "Legacy HAL public token" \
		"${FILE_HAL_FACADE}" "${PATH_HAL_REMOVAL}"
	find "${PATH_HAL_REMOVAL}/srcs/legacy.c" -delete

	mkdir -p "${PATH_HAL_REMOVAL}/srcs/hal/arch/avr8"
	: > "${PATH_HAL_REMOVAL}/srcs/hal/arch/avr8/avr8_stack.h"
	expectFailure hal_removal_relay "Removed HAL relay still exists" \
		"${FILE_HAL_FACADE}" "${PATH_HAL_REMOVAL}"

	expectSuccess compare_create "${FILE_COMPARE}" "${PATH_STAGE_WORK}/manifest" "alpha beta"
	assertFileContains "${PATH_STAGE_WORK}/manifest" "alpha beta"
	VAL_INODE=$(ls -i "${PATH_STAGE_WORK}/manifest" | awk '{ print $1 }')
	expectSuccess compare_keep "${FILE_COMPARE}" "${PATH_STAGE_WORK}/manifest" "alpha beta"
	VAL_INODE_AFTER=$(ls -i "${PATH_STAGE_WORK}/manifest" | awk '{ print $1 }')
	[ "${VAL_INODE}" = "${VAL_INODE_AFTER}" ] || fail "compare_keep: file was replaced"
	expectSuccess compare_change "${FILE_COMPARE}" "${PATH_STAGE_WORK}/manifest" "gamma"
	assertFileContains "${PATH_STAGE_WORK}/manifest" "gamma"
	[ ! -e "${PATH_STAGE_WORK}/manifest.tmp" ] || fail "compare_replace left a temporary file"

	expectFailure path_usage "Usage:" "${FILE_PATH_CHECK}"
	expectFailure path_empty "Empty Make path variable rejected" \
		"${FILE_PATH_CHECK}" -d ""
	expectFailure path_outside "Path outside current directory rejected" \
		"${FILE_PATH_CHECK}" -f "${PATH_PROJECT}/../outside"
	expectFailure path_wrong_type "Invalid -f path rejected" \
		"${FILE_PATH_CHECK}" -f "${PATH_PROJECT}/build"
	expectSuccess path_directory "${FILE_PATH_CHECK}" -d "${PATH_PROJECT}/build"
	expectSuccess path_file "${FILE_PATH_CHECK}" -f "${PATH_PROJECT}/Makefile"

	PATH_GIT_CASE="${PATH_STAGE_WORK}/git_version"
	mkdir -p "${PATH_GIT_CASE}/not_a_repo" "${PATH_GIT_CASE}/repository"
	if ! (cd "${PATH_GIT_CASE}/not_a_repo" && \
		GIT_CEILING_DIRECTORIES="${PATH_GIT_CASE}" "${FILE_VERSION}") \
		> "${PATH_STAGE_WORK}/version_no_repo.log" 2>&1; then
		fail "version_no_repo: command failed"
	fi
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
	[ "$(cat "${PATH_STAGE_WORK}/version_no_repo.log")" = "0.00" ] || \
		fail "version_no_repo: wrong fallback"
	git -C "${PATH_GIT_CASE}/repository" init -q || fail "cannot initialise Git fixture"
	git -C "${PATH_GIT_CASE}/repository" -c user.name=TaskMate \
		-c user.email=test@example.invalid commit -q --allow-empty -m initial || \
		fail "cannot commit Git fixture"
	git -C "${PATH_GIT_CASE}/repository" tag v1.23 || fail "cannot tag Git fixture"
	if ! (cd "${PATH_GIT_CASE}/repository" && "${FILE_VERSION}") \
		> "${PATH_STAGE_WORK}/version_tag.log" 2>&1; then
		fail "version_tag: command failed"
	fi
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
	[ "$(cat "${PATH_STAGE_WORK}/version_tag.log")" = "1.23" ] || \
		fail "version_tag: leading v was not removed"
	git -C "${PATH_GIT_CASE}/repository" -c user.name=TaskMate \
		-c user.email=test@example.invalid commit -q --allow-empty -m post_tag || \
		fail "cannot add post-tag Git fixture commit"
	if ! (cd "${PATH_GIT_CASE}/repository" && "${FILE_VERSION}") \
		> "${PATH_STAGE_WORK}/version_post_tag.log" 2>&1; then
		fail "version_post_tag: command failed"
	fi
	VAL_TEST_COUNT=$((VAL_TEST_COUNT + 1))
	[ "$(cat "${PATH_STAGE_WORK}/version_post_tag.log")" = "1.23" ] || \
		fail "version_post_tag: suffix was not removed"

	printf 'build-system script tests passed: %d cases\n' "${VAL_TEST_COUNT}"
}

writeArchitectureMatrix()
{
	printf '%s\n' \
		'| caller | interfaces | tasks |' \
		'| --- | --- | --- |' \
		'| interfaces | Y | N |' \
		'| tasks | Y | Y |' > "$1"
}

runGuardTests()
{
	stageBegin guards
	FILE_HW="${PATH_PROJECT}/scripts/hardware_target.awk"
	FILE_ARCH="${PATH_PROJECT}/scripts/arch_include.awk"
	FILE_HEADER="${PATH_PROJECT}/scripts/header_allow.awk"

	printf '%s\n' '# targets' 'alpha board mcu arch' 'beta board mcu arch' \
		> "${PATH_STAGE_WORK}/hardware.conf"
	expectSuccess hardware_supported awk -v hardware_target="alpha board mcu arch" \
		-f "${FILE_HW}" "${PATH_STAGE_WORK}/hardware.conf"
	expectSuccess hardware_whitespace awk -v hardware_target="  alpha   board mcu arch " \
		-f "${FILE_HW}" "${PATH_STAGE_WORK}/hardware.conf"
	expectFailure hardware_unsupported "Unsupported hardware target" awk \
		-v hardware_target="gamma board mcu arch" -f "${FILE_HW}" \
		"${PATH_STAGE_WORK}/hardware.conf"

	PATH_ARCH_SRCS="${PATH_STAGE_WORK}/arch_srcs"
	mkdir -p "${PATH_ARCH_SRCS}/interfaces" "${PATH_ARCH_SRCS}/user/tasks"
	writeArchitectureMatrix "${PATH_STAGE_WORK}/matrix.md"
	printf '%s\n' '#include "interfaces/value.h"' > "${PATH_ARCH_SRCS}/user/tasks/ok.c"
	printf '%s\n' '#include "user/tasks/task.h"' > "${PATH_ARCH_SRCS}/interfaces/bad.h"
	printf '%s\n' '#include TASK_HEADER' > "${PATH_ARCH_SRCS}/user/tasks/macro.c"
	expectSuccess architecture_allowed awk -v matrix_file="${PATH_STAGE_WORK}/matrix.md" \
		-v path_sources="${PATH_ARCH_SRCS}" -f "${FILE_ARCH}" \
		"${PATH_STAGE_WORK}/matrix.md" "${PATH_ARCH_SRCS}/user/tasks/ok.c"
	logContains architecture_allowed "Architecture include check passed"
	expectFailure architecture_forbidden "forbidden direct include interfaces -> tasks" awk \
		-v matrix_file="${PATH_STAGE_WORK}/matrix.md" -v path_sources="${PATH_ARCH_SRCS}" \
		-f "${FILE_ARCH}" "${PATH_STAGE_WORK}/matrix.md" \
		"${PATH_ARCH_SRCS}/interfaces/bad.h"
	expectFailure architecture_macro "unverifiable direct include" awk \
		-v matrix_file="${PATH_STAGE_WORK}/matrix.md" -v path_sources="${PATH_ARCH_SRCS}" \
		-f "${FILE_ARCH}" "${PATH_STAGE_WORK}/matrix.md" \
		"${PATH_ARCH_SRCS}/user/tasks/macro.c"
	printf '%s\n' '| caller | interfaces |' '| --- | --- |' '| interfaces | maybe |' \
		> "${PATH_STAGE_WORK}/bad_matrix.md"
	expectFailure architecture_bad_matrix "invalid value maybe" awk \
		-v matrix_file="${PATH_STAGE_WORK}/bad_matrix.md" \
		-v path_sources="${PATH_ARCH_SRCS}" -f "${FILE_ARCH}" \
		"${PATH_STAGE_WORK}/bad_matrix.md" "${PATH_ARCH_SRCS}/user/tasks/ok.c"

	PATH_CONCRETE_SRCS="${PATH_STAGE_WORK}/concrete_srcs"
	FILES_CONCRETE=""
	for FILE_CALLER in \
		system/TaskMate.c system/sysCore/core.c system/sysCall/call.c \
		system/services/service.c tmLibc/lib.c interfaces/api.h user/tasks/task.c
	do
		mkdir -p "${PATH_CONCRETE_SRCS}/${FILE_CALLER%/*}"
		printf '%s\n' '#include "hal/arch/avr8/private.h"' \
			> "${PATH_CONCRETE_SRCS}/${FILE_CALLER}"
		FILES_CONCRETE="${FILES_CONCRETE} ${PATH_CONCRETE_SRCS}/${FILE_CALLER}"
	done
	expectFailure concrete_hal_forbidden "forbidden concrete HAL include" awk \
		-v matrix_file="${PATH_PROJECT}/conf/arch_valid_matrix.md" \
		-v path_sources="${PATH_CONCRETE_SRCS}" -f "${FILE_ARCH}" \
		"${PATH_PROJECT}/conf/arch_valid_matrix.md" ${FILES_CONCRETE}
	for FILE_CALLER in TaskMate.c core.c call.c service.c lib.c api.h task.c
	do
		logContains concrete_hal_forbidden "${FILE_CALLER}:1:"
	done

	mkdir -p "${PATH_CONCRETE_SRCS}/hal/arch/avr8" \
		"${PATH_CONCRETE_SRCS}/user/target/test"
	printf '%s\n' '#include "hal/mcu/atmega2560/private.h"' \
		> "${PATH_CONCRETE_SRCS}/hal/arch/avr8/owner.c"
	printf '%s\n' '#include "hal/arch/avr8/private.h"' \
		> "${PATH_CONCRETE_SRCS}/user/target/test/config.c"
	expectSuccess concrete_hal_owned awk \
		-v matrix_file="${PATH_PROJECT}/conf/arch_valid_matrix.md" \
		-v path_sources="${PATH_CONCRETE_SRCS}" -f "${FILE_ARCH}" \
		"${PATH_PROJECT}/conf/arch_valid_matrix.md" \
		"${PATH_CONCRETE_SRCS}/hal/arch/avr8/owner.c" \
		"${PATH_CONCRETE_SRCS}/user/target/test/config.c"

	PATH_GPIO_GENERATED="${PATH_STAGE_WORK}/gpio_generated"
	mkdir -p "${PATH_GPIO_GENERATED}"
	printf 'typedef enum { GPIO_SIGNAL_TEST, GPIO_SIGNAL_COUNT } gpio_signal_t;\n' \
		> "${PATH_GPIO_GENERATED}/gpio_signals.inc"
	printf '#include "interfaces/gpio_signals.h"\n' > "${PATH_STAGE_WORK}/gpio_header.c"
	expectSuccess gpio_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -I "${PATH_GPIO_GENERATED}" -fsyntax-only \
		"${PATH_STAGE_WORK}/gpio_header.c"

	printf '#include "interfaces/hal_atomic.h"\n' > "${PATH_STAGE_WORK}/atomic_header.c"
	expectSuccess atomic_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/atomic_header.c"

	printf '#include "interfaces/hal_context.h"\nhal_context_t *context;\n' \
		> "${PATH_STAGE_WORK}/context_header.c"
	expectSuccess context_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/context_header.c"

	PATH_HEADER_SRCS="${PATH_STAGE_WORK}/header_srcs"
	mkdir -p "${PATH_HEADER_SRCS}/allowed" "${PATH_HEADER_SRCS}/forbidden"
	printf '%s\n' '#include "critical.h"' > "${PATH_HEADER_SRCS}/allowed/use.c"
	printf '%s\n' '{' 'source_file critical.h' 'allow {' 'allowed/use.c' '}' '}' \
		> "${PATH_STAGE_WORK}/header.conf"
	expectSuccess header_allowed awk -v PATH_SOURCES="${PATH_HEADER_SRCS}" \
		-v h_check_log="${PATH_STAGE_WORK}/header_allowed.report" -f "${FILE_HEADER}" \
		"${PATH_STAGE_WORK}/header.conf"
	assertFileContains "${PATH_STAGE_WORK}/header_allowed.report" "[  OK  ]"
	printf '%s\n' '#include "critical.h"' > "${PATH_HEADER_SRCS}/forbidden/use.c"
	expectFailure header_forbidden "Forbidden include detected" awk \
		-v PATH_SOURCES="${PATH_HEADER_SRCS}" \
		-v h_check_log="${PATH_STAGE_WORK}/header_forbidden.report" -f "${FILE_HEADER}" \
		"${PATH_STAGE_WORK}/header.conf"
	printf '%s\n' '{' 'source_file critical.h' > "${PATH_STAGE_WORK}/bad_header.conf"
	expectFailure header_bad_configuration "missing closing" awk \
		-v PATH_SOURCES="${PATH_HEADER_SRCS}" \
		-v h_check_log="${PATH_STAGE_WORK}/bad_header.report" -f "${FILE_HEADER}" \
		"${PATH_STAGE_WORK}/bad_header.conf"

	printf 'build-system guard tests passed: %d cases\n' "${VAL_TEST_COUNT}"
}

runReportTests()
{
	stageBegin reports
	printf '%s\n' \
		'language files blank comment code' \
		'C 1 2 3 10' \
		'C/C++ Header 1 4 5 20' \
		'make 1 1 2 7' \
		'awk 1 3 4 6' \
		'Markdown 1 5 0 15' > "${PATH_STAGE_WORK}/cloc.raw"
	expectSuccess cloc_data awk -v file="${PATH_STAGE_WORK}/cloc.data" \
		-f "${PATH_PROJECT}/scripts/cloc_data.awk" "${PATH_STAGE_WORK}/cloc.raw"
	assertFileContains "${PATH_STAGE_WORK}/cloc.data" "code_total 67"
	assertFileContains "${PATH_STAGE_WORK}/cloc.data" "make_total 23"
	expectSuccess cloc_show awk -f "${PATH_PROJECT}/scripts/cloc_show.awk" \
		"${PATH_STAGE_WORK}/cloc.data"
	logContains cloc_show "code         : 67 loc"
	expectSuccess cloc_summary awk -f "${PATH_PROJECT}/scripts/build_summary_cloc.awk" \
		"${PATH_STAGE_WORK}/cloc.data"
	logContains cloc_summary "lines of code    : 67"

	printf '%s\n' 'text data bss dec hex filename' '1000 24 200 1224 4c8 TaskMate.elf' \
		> "${PATH_STAGE_WORK}/memory.raw"
	expectSuccess memory_data awk -v flash_total_k=64 -v ram_total_k=8 \
		-v output_file="${PATH_STAGE_WORK}/memory.data" \
		-f "${PATH_PROJECT}/srcs/hal/arch/avr8/avr8_memory_data.awk" \
		"${PATH_STAGE_WORK}/memory.raw"
	assertFileContains "${PATH_STAGE_WORK}/memory.data" "Flash 1024 65536"
	assertFileContains "${PATH_STAGE_WORK}/memory.data" "RAM 224 8192"
	expectSuccess memory_show awk \
		-f "${PATH_PROJECT}/srcs/hal/arch/avr8/avr8_memory_show.awk" \
		"${PATH_STAGE_WORK}/memory.data"
	logContains memory_show "1024 / 65536 bytes"

	printf '%s\n' 'Memory used total %' 'Flash 1 100 85.0' 'RAM 86 100 86.0' \
		> "${PATH_STAGE_WORK}/memory_warning.data"
	expectSuccess memory_warning awk -f "${PATH_PROJECT}/scripts/build_summary_memory.awk" \
		"${PATH_STAGE_WORK}/memory_warning.data"
	logContains memory_warning "WARNING: usage high > 85%"
	printf '%s\n' 'Memory used total %' 'Flash 85 100 85.0' 'RAM 98 100 98.0' \
		> "${PATH_STAGE_WORK}/memory_boundaries.data"
	expectSuccess memory_boundaries awk \
		-f "${PATH_PROJECT}/scripts/build_summary_memory.awk" \
		"${PATH_STAGE_WORK}/memory_boundaries.data"
	logContains memory_boundaries "WARNING: usage high > 85%"
	logExcludes memory_boundaries "ERROR:"
	printf '%s\n' 'Memory used total %' 'Flash 99 100 99.0' \
		> "${PATH_STAGE_WORK}/memory_error.data"
	expectFailure memory_error "ERROR: usage high > 98%" awk \
		-f "${PATH_PROJECT}/scripts/build_summary_memory.awk" \
		"${PATH_STAGE_WORK}/memory_error.data"

	printf '%s\n' 'public_target:' '#help [group] Public target.' '_private_target:' \
		> "${PATH_STAGE_WORK}/help.mk"
	expectSuccess help_report awk -f "${PATH_PROJECT}/scripts/make_help.awk" \
		"${PATH_STAGE_WORK}/help.mk"
	logContains help_report "public_target:"
	logContains help_report "[group] Public target."
	logExcludes help_report "_private_target:"

	printf '%s\n' \
		'[fileUtility.c] info : keep old.c' \
		'[fileUtility.c] info : change new.c' \
		'[fileUtility.c] info : * summary *' > "${PATH_STAGE_WORK}/autoCode.log"
	expectSuccess autocode_log awk -v log_file="${PATH_STAGE_WORK}/autoCode.log" \
		-f "${PATH_PROJECT}/scripts/autocode_log.awk" "${PATH_STAGE_WORK}/autoCode.log"
	logContains autocode_log "keep old.c"
	logContains autocode_log "change new.c"
	logContains autocode_log "* summary *"
	assertFileContains "${PATH_STAGE_WORK}/autoCode.log" "keep old.c"
	expectSuccess integrated_help bmake -C "${PATH_PROJECT}" help
	logContains integrated_help "test_build_system:"
	logContains integrated_help "Run the complete build-system black-box test corpus."

	printf 'build-system report tests passed: %d cases\n' "${VAL_TEST_COUNT}"
}

case "${VAL_STAGE}" in
configuration) runConfigurationTests ;;
scripts) runScriptTests ;;
guards) runGuardTests ;;
reports) runReportTests ;;
all)
	runConfigurationTests
	runScriptTests
	runGuardTests
	runReportTests
	;;
*) fail "unknown stage <${VAL_STAGE}>" ;;
esac
