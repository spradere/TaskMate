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
# Description: Run one stage or the complete build-system test corpus.
#
# Inputs: Stage name, project directory, and test work directory.
# Outputs: Test progress and diagnostics, work fixtures, and nonzero status on failure.
# ------------------------------------------------------------------------------

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

assertWordsSorted()
{
	tr ' ' '\n' < "${PATH_STAGE_WORK}/$1.log" | sed '/^$/d' \
		> "${PATH_STAGE_WORK}/$1.words"
	LC_ALL=C sort -u "${PATH_STAGE_WORK}/$1.words" > "${PATH_STAGE_WORK}/$1.sorted"
	if ! cmp -s "${PATH_STAGE_WORK}/$1.words" "${PATH_STAGE_WORK}/$1.sorted"; then
		fail "$1: paths are not sorted or contain duplicates"
	fi
}

assertFileNamesSorted()
{
	tr ' ' '\n' < "${PATH_STAGE_WORK}/$1.log" | sed '/^$/d' \
		> "${PATH_STAGE_WORK}/$1.words"
	awk '{ path = $0; name = path; sub(/^.*\//, "", name); print name "|" path }' \
		"${PATH_STAGE_WORK}/$1.words" > "${PATH_STAGE_WORK}/$1.keys"
	LC_ALL=C sort "${PATH_STAGE_WORK}/$1.keys" > "${PATH_STAGE_WORK}/$1.sorted"
	if ! cmp -s "${PATH_STAGE_WORK}/$1.keys" "${PATH_STAGE_WORK}/$1.sorted"; then
		fail "$1: filenames are not sorted"
	fi
}

targetMake()
{
	bmake -C "${PATH_PROJECT}" VAL_TARGET=test1 "$@"
}

assembleAvr8ContextMacros()
{
	(
		cd "${PATH_STAGE_WORK}" || exit 1
		avr-gcc -mmcu=atmega2560 -x assembler-with-cpp \
			-c avr8_context_macros.S -o avr8_context_macros.o
	)
}

assembleAt2560TimerContextMacros()
{
	(
		cd "${PATH_STAGE_WORK}" || exit 1
		avr-gcc -mmcu=atmega2560 -x assembler-with-cpp \
			-c at2560_timer_context_macros.S -o at2560_timer_context_macros.o
	)
}

runConfigurationTests()
{
	stageBegin configuration

	expectFailure no_target_build "don't know how to make all" bmake -C "${PATH_PROJECT}" all
	expectSuccess no_target_help bmake -C "${PATH_PROJECT}" \
		PATHS_SOURCE_SEARCH=path_that_must_not_be_searched help
	expectOutput default_environment "freebsd" bmake -C "${PATH_PROJECT}" \
		-V OPT_ENVIRONMENT
	expectOutput linux_environment "linux" bmake -C "${PATH_PROJECT}" HOST=linux \
		-V OPT_ENVIRONMENT
	expectOutput cygwin_environment "w10-cygwin" bmake -C "${PATH_PROJECT}" \
		HOST=w10-cygwin -V OPT_ENVIRONMENT
	expectFailure invalid_environment \
		'>>> Invalid option OPT_ENVIRONMENT : "invalid". Valid values: [ freebsd | linux | w10-cygwin ] <<<' \
		bmake -C "${PATH_PROJECT}" HOST=invalid -V OPT_ENVIRONMENT
	expectOutput verbose_default "0" bmake -C "${PATH_PROJECT}" -V OPT_VERBOSE_LEVEL
	expectOutput verbose_linux "0" bmake -C "${PATH_PROJECT}" HOST=linux \
		-V OPT_VERBOSE_LEVEL
	expectOutput verbose_enabled "1" bmake -C "${PATH_PROJECT}" VERBOSE=1 \
		-V OPT_VERBOSE_LEVEL
	expectOutput verbose_level_two "2" bmake -C "${PATH_PROJECT}" VERBOSE=2 \
		-V OPT_VERBOSE_LEVEL
	expectFailure verbose_not_numeric \
		'>>> Invalid option for OPT_VERBOSE_LEVEL : "invalid", should be a number. <<<' \
		bmake -C "${PATH_PROJECT}" \
		VERBOSE=invalid -V OPT_VERBOSE_LEVEL
	expectFailure verbose_out_of_range \
		'>>> Invalid option for OPT_VERBOSE_LEVEL : "3", should be [ 0 | 1 ]. <<<' \
		bmake -C "${PATH_PROJECT}" VERBOSE=3 -V OPT_VERBOSE_LEVEL
	expectOutput taskmate_version "0.33" targetMake -V VAL_TM_VERSION
	expectOutput taskmate_version_major "0" targetMake -V VAL_TM_VER_MAJOR
	expectOutput taskmate_version_minor "33" targetMake -V VAL_TM_VER_MINOR
	expectOutput build_counter "1324" targetMake -V VAL_BUILD_CNT
	printf '%s\n' 'VAL_TM_VERSION = 7.42' > "${PATH_STAGE_WORK}/version.conf"
	printf '%s\n' 'VAL_BUILD_CNT = 321' > "${PATH_STAGE_WORK}/counter.conf"
	expectOutput configured_taskmate_version "7.42" targetMake \
		CONF_TASKMATE_VERSION="${PATH_STAGE_WORK}/version.conf" \
		CONF_BUILD_COUNTER="${PATH_STAGE_WORK}/counter.conf" -V VAL_TM_VERSION
	expectOutput configured_build_counter "321" targetMake \
		CONF_TASKMATE_VERSION="${PATH_STAGE_WORK}/version.conf" \
		CONF_BUILD_COUNTER="${PATH_STAGE_WORK}/counter.conf" -V VAL_BUILD_CNT
	assertFileExcludes "${PATH_PROJECT}/conf/programs-list.conf" "git"
	expectSuccess all_without_vcs_metadata targetMake -n \
		FILE_GIT_IGNORE="${PATH_STAGE_WORK}/generated.gitignore" all
	logExcludes all_without_vcs_metadata "git describe"
	logExcludes all_without_vcs_metadata "generated.gitignore"
	expectSuccess autocode_source_order bmake -C "${PATH_PROJECT}" \
		-V FILES_AUTOCODE_SRC_ALL
	assertWordsSorted autocode_source_order
	logContains autocode_source_order "srcs/autoCode/autoCode.c"
	logContains autocode_source_order "srcs/autoCode/autoCode.h"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters.h"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters_drivers.c"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters_errors.c"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters_gpio.c"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters_modules.c"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters_scli.c"
	logContains autocode_source_order "srcs/autoCode/tagWriters/tagWriters_threads.c"
	expectSuccess autocode_editor_order bmake -C "${PATH_PROJECT}" \
		-V FILES_EDITOR_AUTOCODE
	assertFileNamesSorted autocode_editor_order
	expectSuccess taskmate_source_order bmake -C "${PATH_PROJECT}" \
		-V FILES_NOTARGET_SRC_ALL
	assertWordsSorted taskmate_source_order
	logContains taskmate_source_order "srcs/system/boot.c"
	logExcludes taskmate_source_order "srcs/autoCode/"
	expectSuccess taskmate_editor_order bmake -C "${PATH_PROJECT}" \
		-V FILES_EDITOR_TM
	assertFileNamesSorted taskmate_editor_order
	expectSuccess makefile_order bmake -C "${PATH_PROJECT}" -V FILES_MK
	assertWordsSorted makefile_order
	logContains makefile_order "./Makefile"
	logContains makefile_order "./test/build_test.mk"
	expectSuccess makefile_editor_order bmake -C "${PATH_PROJECT}" \
		-V FILES_EDITOR_MK
	assertFileNamesSorted makefile_editor_order
	expectSuccess documentation_editor_order bmake -C "${PATH_PROJECT}" \
		-V FILES_EDITOR_DOC
	assertFileNamesSorted documentation_editor_order
	logContains documentation_editor_order "doc/arch_notes/build.md"
	logContains documentation_editor_order "doc/howto_doxygen.txt"
	expectSuccess selected_target_autocode_test bmake -C "${PATH_PROJECT}" -n \
		TARGET=test1 FILE_AUTOCODE_TARGET="${PATH_STAGE_WORK}/autoCode" test_autoCode
	logContains selected_target_autocode_test "clang -DAUTOCODE_BUILD"
	expectOutput autocode_sanitize_target "build/autoCode_sanitize" \
		bmake -C "${PATH_PROJECT}" -V FILE_AUTOCODE_TEST_SANITIZE_TARGET
	expectOutput clang_tidy_command "clang-tidy19" bmake -C "${PATH_PROJECT}" \
		-V FILE_CLANG_TIDY
	expectOutput cppcheck_command "cppcheck" bmake -C "${PATH_PROJECT}" \
		-V FILE_CPPCHECK
	assertFileContains "${PATH_PROJECT}/.clang-tidy" "  readability-*,"
	assertFileExcludes "${PATH_PROJECT}/.clang-tidy" "  -readability-magic-numbers,"
	assertFileContains "${PATH_PROJECT}/.clang-tidy" \
		"WarningsAsErrors: 'readability-magic-numbers'"
	assertFileContains "${PATH_PROJECT}/.clang-tidy" \
		"  - key: readability-magic-numbers.IgnoredIntegerValues"
	assertFileContains "${PATH_PROJECT}/.clang-tidy" "    value: '1'"
	expectSuccess autocode_tidy_recipe targetMake -n \
		FILE_CLANG_TIDY=taskmate-test-clang-tidy tidy_autocode
	logContains autocode_tidy_recipe "taskmate-test-clang-tidy"
	logContains autocode_tidy_recipe "-std=c17"
	logExcludes autocode_tidy_recipe "srcs/autoCode/autoCode.h --"
	expectSuccess autocode_cppcheck_recipe targetMake -n \
		FILE_CPPCHECK=taskmate-test-cppcheck cppcheck_autocode
	logContains autocode_cppcheck_recipe "taskmate-test-cppcheck"
	logContains autocode_cppcheck_recipe "--platform=unix64"
	logContains autocode_cppcheck_recipe "--library=bsd"
	logContains autocode_cppcheck_recipe "-DAUTOCODE_BUILD"
	logExcludes autocode_cppcheck_recipe "--force"
	expectSuccess no_target_autocode_sanitize bmake -C "${PATH_PROJECT}" -n \
		FILE_AUTOCODE_TEST_SANITIZE_TARGET="${PATH_STAGE_WORK}/autoCode_sanitize" \
		_test_ac_sanitize
	logContains no_target_autocode_sanitize "clang -DAUTOCODE_BUILD -Isrcs/"
	expectSuccess unavailable_autocode_sanitize bmake -C "${PATH_PROJECT}" \
		FILE_AUTOCODE_TEST_SANITIZE_CC=false test_ac_sanitize
	logContains unavailable_autocode_sanitize \
		"Skipping autoCode sanitizer tests: unsupported by Clang"
	expectOutput freebsd_usb_key "/media/usbkey" bmake -C "${PATH_PROJECT}" \
		HOST=freebsd -V PATH_USBKEY
	expectOutput freebsd_usb_device "/dev/da0s1" bmake -C "${PATH_PROJECT}" \
		HOST=freebsd -V FILE_USBDEV
	expectOutput linux_usb_key "" bmake -C "${PATH_PROJECT}" HOST=linux -V PATH_USBKEY
	expectOutput linux_usb_device "" bmake -C "${PATH_PROJECT}" HOST=linux -V FILE_USBDEV
	expectSuccess freebsd_backup_dry_run bmake -C "${PATH_PROJECT}" -n \
		HOST=freebsd backup
	logContains freebsd_backup_dry_run "fstyp -l /dev/da0s1"
	logContains freebsd_backup_dry_run 'if [ "$label" != "TASKMATE" ]'
	logContains freebsd_backup_dry_run 'mount | grep -q " on /media/usbkey "'
	logContains freebsd_backup_dry_run "rsync -av ./"
	expectSuccess linux_backup_dry_run bmake -C "${PATH_PROJECT}" -n HOST=linux backup
	logExcludes linux_backup_dry_run "fstyp"
	logExcludes linux_backup_dry_run "mount | grep"
	logExcludes linux_backup_dry_run "rsync"
	expectOutput short_target_stack "test1 arduinoMega atmega2560 avr8" \
		bmake -C "${PATH_PROJECT}" TARGET=test1 -V VAL_HW_STACK
	expectOutput default_stack "test1 arduinoMega atmega2560 avr8" \
		targetMake -V VAL_HW_STACK
	expectOutput default_path "build/test1_arduinoMega_atmega2560_avr8" \
		targetMake -V PATH_BUILD_TARGET
	expectOutput architecture_compiler "srcs/hal/arch/avr8/avr8_CC.mk" \
		targetMake -V FILE_ARCH_CC
	expectSuccess taskmate_tidy_flags targetMake -V CFLAGS_CLANG_TIDY
	logContains taskmate_tidy_flags "--target=avr"
	logContains taskmate_tidy_flags "-mmcu=atmega2560"
	expectSuccess taskmate_tidy_sources targetMake -V FILES_CLANG_TIDY_SRC
	logExcludes taskmate_tidy_sources "srcs/hal/arch/avr8/avr8_context.c"
	expectSuccess taskmate_cppcheck_settings targetMake \
		FILE_CPPCHECK=taskmate-test-cppcheck -V FILE_CPPCHECK \
		-V OPT_CPPCHECK_TARGET -V CFLAGS_CPPCHECK
	logContains taskmate_cppcheck_settings "taskmate-test-cppcheck"
	logContains taskmate_cppcheck_settings "--platform=avr8"
	logContains taskmate_cppcheck_settings "--library=avr"
	logContains taskmate_cppcheck_settings "-DPROGMEM="
	logExcludes taskmate_cppcheck_settings "--force"
	expectOutput architecture_types_header \
		"srcs/hal/arch/avr8/avr8_types.h" \
		targetMake -V FILE_HAL_ARCH_TYPES
	expectSuccess architecture_types_global_compile_flag targetMake -V CFLAGS
	logExcludes architecture_types_global_compile_flag \
		"-include srcs/hal/arch/avr8/avr8_types.h"
	expectOutput architecture_types_selected_compile_flag \
		"-include srcs/hal/arch/avr8/avr8_types.h" \
		targetMake -V CFLAGS_srcs/system/sysCore/sys_threads.c
	expectOutput architecture_types_scheduler_compile_flag \
		"-include srcs/hal/arch/avr8/avr8_types.h" \
		targetMake -V CFLAGS_srcs/system/sysCore/sys_scheduler.c
	for VAL_SOURCE in system/boot.c system/sysCall/sc_driver.c system/sysCall/sc_gpio.c \
		system/sysCall/sc_threads.c
	do
		VAL_ARCH_TEST_NAME=$(printf '%s' "${VAL_SOURCE}" | tr '/.' '__')
		VAL_ARCH_TEST_NAME="architecture_types_unselected_${VAL_ARCH_TEST_NAME}"
		expectSuccess "${VAL_ARCH_TEST_NAME}" \
			targetMake -V "CFLAGS_srcs/${VAL_SOURCE}"
		logExcludes "${VAL_ARCH_TEST_NAME}" \
			"-include srcs/hal/arch/avr8/avr8_types.h"
	done
	expectOutput string_macro_selected_compile_flag \
		"-include srcs/hal/arch/avr8/avr8_string_macro.h" \
		targetMake -V CFLAGS_srcs/system/sysCall/sc_driver.c
	expectSuccess autocode_generated_inputs targetMake -V FILES_AUTOCODE_INC
	logExcludes autocode_generated_inputs "find "
	logExcludes autocode_generated_inputs "-path"
	expectOutput missing_autocode_generated_inputs "" targetMake \
		PATH_BUILD_TARGET="${PATH_STAGE_WORK}/missing_target" -V FILES_AUTOCODE_INC
	expectSuccess autocode_tag_inputs targetMake -V FILES_PARSE_TAG
	logContains autocode_tag_inputs "srcs/system/sysCall/sc_errors.c"
	logContains autocode_tag_inputs "srcs/interfaces/error_catalog.h"
	logContains autocode_tag_inputs "srcs/hal/mcu/atmega2560/at2560_gpio.c"
	logExcludes autocode_tag_inputs "srcs/autoCode/"
	expectOutput cpu_frequency "16000000UL" targetMake -V VAL_CPU_FREQ
	expectOutput mcu_serial "atmega2560" targetMake -V VAL_MCU_SERIAL

	expectOutput z600_stack "z600 pc freebsd ucontext" \
		bmake -C "${PATH_PROJECT}" VAL_TARGET=z600 -V VAL_HW_STACK
	expectOutput z600_compiler "srcs/hal/host/freebsd/freebsd_CC.mk" \
		bmake -C "${PATH_PROJECT}" VAL_TARGET=z600 -V FILE_ARCH_CC
	expectSuccess freebsd_tidy_target bmake -C "${PATH_PROJECT}" VAL_TARGET=z600 \
		-V FILE_CLANG_TIDY_TARGET
	VAL_FREEBSD_TIDY_TARGET=$(cat "${PATH_STAGE_WORK}/freebsd_tidy_target.log")
	expectSuccess freebsd_tidy_flags bmake -C "${PATH_PROJECT}" VAL_TARGET=z600 \
		-V CFLAGS_CLANG_TIDY
	logContains freebsd_tidy_flags "--target=${VAL_FREEBSD_TIDY_TARGET}"
	expectSuccess freebsd_cppcheck_settings bmake -C "${PATH_PROJECT}" VAL_TARGET=z600 \
		FILE_CPPCHECK=taskmate-test-cppcheck -V FILE_CPPCHECK \
		-V OPT_CPPCHECK_TARGET -V CFLAGS_CPPCHECK
	logContains freebsd_cppcheck_settings "taskmate-test-cppcheck"
	logContains freebsd_cppcheck_settings "--platform=unix64"
	logContains freebsd_cppcheck_settings "--library=bsd"
	logContains freebsd_cppcheck_settings "--library=posix"
	expectOutput z600_types_header \
		"srcs/hal/host/ucontext/ucontext_types.h" \
		bmake -C "${PATH_PROJECT}" VAL_TARGET=z600 -V FILE_HAL_ARCH_TYPES
	expectSuccess z600_compile_sources bmake -C "${PATH_PROJECT}" \
		VAL_TARGET=z600 -V FILES_COMPILE_SRC
	logContains z600_compile_sources "srcs/hal/host/ucontext/ucontext_context.c"
	logExcludes z600_compile_sources "srcs/system/sysCall/sc_gpio.c"
	logExcludes z600_compile_sources "srcs/user/tasks/task1.c"
	logExcludes z600_compile_sources "srcs/user/tasks/task2.c"
	logContains z600_compile_sources "srcs/user/target/z600/z600_task1.c"
	logContains z600_compile_sources "srcs/user/target/z600/z600_task2.c"
	logContains z600_compile_sources "srcs/hal/host/freebsd/freebsd_timerSched.c"
	logContains z600_compile_sources "srcs/hal/host/freebsd/freebsd_timerMonotonic.c"
	logContains z600_compile_sources "srcs/hal/host/freebsd/freebsd_usart.c"
	logContains z600_compile_sources "srcs/system/services/scli.c"
	logContains z600_compile_sources "srcs/system/services/commands/scli_driver.c"
	logContains z600_compile_sources "srcs/system/services/commands/scli_stack.c"
	logContains z600_compile_sources "srcs/system/services/commands/scli_thread.c"
	logExcludes z600_compile_sources "srcs/system/services/commands/scli_date.c"
	logExcludes z600_compile_sources "srcs/system/services/commands/scli_i2c.c"
	logExcludes z600_compile_sources "srcs/hal/arch/"
	logExcludes z600_compile_sources "srcs/hal/mcu/"
	expectOutput z600_gpio_signals "" bmake -C "${PATH_PROJECT}" \
		VAL_TARGET=z600 -V FILE_GPIO_SIGNALS
	expectOutput z600_gpio_wiring "" bmake -C "${PATH_PROJECT}" \
		VAL_TARGET=z600 -V FILE_WIREGPIO
	expectFailure gpio_signals_without_wiring \
		'>>> GPIO file for signals and wiring must both be set <<<' \
		targetMake FILE_WIREGPIO= -V FILES_COMPILE_SRC
	expectFailure gpio_wiring_without_signals \
		'>>> GPIO file for signals and wiring must both be set <<<' \
		targetMake FILE_GPIO_SIGNALS= -V FILES_COMPILE_SRC
	expectSuccess z600_tag_inputs bmake -C "${PATH_PROJECT}" \
		VAL_TARGET=z600 -V FILES_PARSE_TAG
	logExcludes z600_tag_inputs "gpio_common.h"
	expectSuccess z600_generated_inputs bmake -C "${PATH_PROJECT}" \
		VAL_TARGET=z600 -V FILES_AUTOCODE_INC
	logExcludes z600_generated_inputs "gpio_signals.inc"
	logExcludes z600_generated_inputs "wire_gpio.inc"

	expectSuccess default_compile_sources targetMake -V FILES_COMPILE_SRC
	logContains default_compile_sources "srcs/hal/arch/avr8/avr8_atomic.c"
	logContains default_compile_sources "srcs/hal/arch/avr8/avr8_contextStart.S"
	logContains default_compile_sources "srcs/hal/arch/avr8/avr8_delay.c"
	logContains default_compile_sources "srcs/hal/mcu/atmega2560/at2560_timerContext.S"
	logContains default_compile_sources "srcs/hal/drivers/lcd/lcd_AMC2004.c"
	logContains default_compile_sources "srcs/system/services/commands/scli_date.c"
	logContains default_compile_sources "srcs/system/services/commands/scli_stack.c"

	expectSuccess default_initrc_sources targetMake -V FILES_INITRC_SRC
	logContains default_initrc_sources "srcs/system/services/scli.c"
	logContains default_initrc_sources "srcs/system/services/commands/scli_date.c"
	logContains default_initrc_sources "srcs/system/services/commands/scli_stack.c"
	logContains default_initrc_sources "srcs/hal/mcu/atmega2560/at2560_gpio.c"
	logContains default_initrc_sources "srcs/hal/mcu/atmega2560/at2560_timerContext.c"
	logContains default_initrc_sources "srcs/hal/mcu/atmega2560/at2560_timerMonotonic.c"
	expectOutput default_initrc_dirs "" \
		targetMake -V PATHS_INITRC_SOURCES

	expectOutput default_extra_sources \
		"srcs/hal/arch/avr8/avr8_contextStart.S srcs/hal/mcu/atmega2560/at2560_timerContext.S" \
		targetMake -V FILES_EXTRA_SRC

	expectOutput initrc_directory_sources "" targetMake -V FILES_INITRC_DIR_SRC

	expectSuccess object_mapping targetMake -V FILES_OBJ
	logContains object_mapping \
		"build/test1_arduinoMega_atmega2560_avr8/srcs/system/boot.o"
	logContains object_mapping \
		"build/test1_arduinoMega_atmega2560_avr8/srcs/hal/arch/avr8/avr8_contextStart_asm.o"
	logContains object_mapping \
		"build/test1_arduinoMega_atmega2560_avr8/srcs/hal/mcu/atmega2560/at2560_timerContext_asm.o"
	logExcludes object_mapping ".c.o"
	logExcludes object_mapping ".S.o"

	for VAL_PATH_LIST in \
		PATHS_SOURCE_SEARCH PATHS_EXTRA_SRC FILES_EXTRA_SRC FILES_SRC_H \
		FILES_DRIVER_INTERFACES FILES_INITRC FILES_INITRC_SRC PATHS_INITRC_SOURCES \
		FILES_INITRC_DIR_SRC FILES_BASE_SYSTEM_SRC FILES_COMPILE_SRC FILES_COMPILE_C \
		FILES_COMPILE_S FILES_OBJ_C FILES_OBJ_S FILES_OBJ FILES_DEP FILES_ERROR \
		FILES_AUTOCODE_SRC FILES_AUTOCODE_SRC_H \
		FILES_AUTOCODE_SRC_ALL FILES_NOTARGET_SRC FILES_NOTARGET_SRC_H \
		FILES_NOTARGET_SRC_ALL FILES_DOC FILES_MK_MK FILES_MK_HAL FILES_MK_TEST \
		FILES_MK FILES_AUTOCODE_INC FILES_PARSE_TAG PATHS_TM_STRING_ALLOWED
	do
		expectSuccess "path_list_${VAL_PATH_LIST}" targetMake -V "${VAL_PATH_LIST}"
		assertWordsSorted "path_list_${VAL_PATH_LIST}"
	done
	expectFailure duplicate_direct_path \
		'>>> Duplicate path in FILES_AUTOCODE_SRC: duplicate.c duplicate.c <<<' \
		bmake -C "${PATH_PROJECT}" FILES_AUTOCODE_SRC='duplicate.c duplicate.c' \
		-V FILES_AUTOCODE_SRC
	expectFailure duplicate_composed_path '>>> Duplicate path in FILES_COMPILE_SRC:' \
		targetMake FILES_EXTRA_SRC=srcs/system/boot.c -V FILES_COMPILE_SRC
	logContains duplicate_composed_path ' <<<'
	printf '%s\n' \
		'addModule task missing -source_file user/tasks/missing_from_initrc.c' \
		> "${PATH_STAGE_WORK}/missing_source_init.rc"
	expectFailure initrc_missing_source \
		'>>> File not found srcs/user/tasks/missing_from_initrc.c <<<' \
		targetMake FILES_INITRC="${PATH_STAGE_WORK}/missing_source_init.rc" \
		-V FILES_COMPILE_SRC
	printf '%s\n' \
		'.include "srcs/user/target/test1/test1.mk"' \
		'FILES_EXTRA_SRC += /etc/passwd' \
		> "${PATH_STAGE_WORK}/invalid_source_target.mk"
	expectFailure target_source_outside \
		'>>> path check failed for file /etc/passwd <<<' \
		targetMake FILE_TARGET_MK="${PATH_STAGE_WORK}/invalid_source_target.mk" \
		-V FILES_COMPILE_SRC
	VAL_DRIVER_INTERFACES="srcs/interfaces/drv_gpio.h srcs/interfaces/drv_i2c.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_lcd.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_rtc.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_timerContext.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_timerMonotonic.h"
	VAL_DRIVER_INTERFACES="${VAL_DRIVER_INTERFACES} srcs/interfaces/drv_usart.h"
	expectOutput driver_interface_order "${VAL_DRIVER_INTERFACES}" \
		targetMake -V FILES_DRIVER_INTERFACES

	expectFailure invalid_target \
		'>>> Target makefile not found srcs/user/target/missing/missing.mk <<<' \
		bmake -C "${PATH_PROJECT}" VAL_TARGET=missing -V VAL_HW_STACK
	expectFailure invalid_option \
		'>>> Invalid option OPT_CLEAN_AUTOCODE_LOGS : "invalid". Valid values: [ yes | no ] <<<' \
		targetMake OPT_CLEAN_AUTOCODE_LOGS=invalid -V VAL_HW_STACK

	PATH_MANIFESTS="${PATH_STAGE_WORK}/manifests"
	: > "${PATH_STAGE_WORK}/programs.conf"
	expectSuccess dependency_manifests targetMake \
		PATH_BUILD_TARGET="${PATH_MANIFESTS}" \
		FILE_TM_INFO="${PATH_STAGE_WORK}/tm_info.h" \
		FILE_BUILD_INFO="${PATH_STAGE_WORK}/build_info.txt" \
		CONF_PROGRAMS_LIST="${PATH_STAGE_WORK}/programs.conf" \
		FILE_PROGRAMS_CHECK_STAMP="${PATH_STAGE_WORK}/programs.stamp" \
		FILE_AVR8_PROGRAMS_LIST="${PATH_STAGE_WORK}/programs.conf" \
		FILE_AVR8_PROGRAMS_CHECK_STAMP="${PATH_STAGE_WORK}/avr8_programs.stamp" \
		_autocode_dependency_check
	assertFileContains "${PATH_MANIFESTS}/gpio_signals.deps" \
		"srcs/user/target/test1/test1_signals.gpio"
	expectFailure wire_gpio_outside "Path outside current directory rejected" \
		targetMake \
		FILE_WIREGPIO="/etc/passwd" \
		_autocode_dependency_check
	expectFailure wire_gpio_wrong_type "Invalid -f path rejected" \
		targetMake FILE_WIREGPIO="${PATH_PROJECT}/build" \
		_autocode_dependency_check

	PATH_DEPENDENCIES="${PATH_STAGE_WORK}/dependencies"
	mkdir -p "${PATH_DEPENDENCIES}"
	printf 'fixture.o: fixture.c \\\r\n fixture.h\r\n' > "${PATH_DEPENDENCIES}/fixture.d"
	expectSuccess dependencies_crlf_first targetMake \
		PATH_BUILD_TARGET="${PATH_DEPENDENCIES}" \
		FILES_DEP="${PATH_DEPENDENCIES}/fixture.d" \
		FILE_DEPS_ALL="${PATH_DEPENDENCIES}/.deps.d" _dependency
	expectSuccess dependencies_crlf_second targetMake \
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
	FILE_BUNDLE="${PATH_PROJECT}/scripts/bundle.sh"
	FILE_DEL_BRANCH="${PATH_PROJECT}/scripts/del_branch.sh"
	FILE_AUTOCODE_VERSION="${PATH_PROJECT}/scripts/autocode_version.awk"
	FILE_INITRC_SOURCES="${PATH_PROJECT}/scripts/initrc_sources.awk"

	printf '%s\n' '#define AUTOCODE_VERSION_INITRCMAJOR 1U' \
		'#define AUTOCODE_VERSION_INITRCMINOR 13U' \
		'#define AUTOCODE_VERSION_MAJOR 1U' \
		'#define AUTOCODE_VERSION_MINOR 9U' > "${PATH_STAGE_WORK}/autoCode.h"
	expectSuccess autocode_version_match awk -v expected_major=1 -v expected_minor=9 \
		-f "${FILE_AUTOCODE_VERSION}" "${PATH_STAGE_WORK}/autoCode.h"
	expectOutput autocode_version_report "autoCode : 1.9
initrc : 1.13" awk -v expected_major=1 -v expected_minor=9 -v report_versions=1 \
		-f "${FILE_AUTOCODE_VERSION}" "${PATH_STAGE_WORK}/autoCode.h"
	expectFailure autocode_version_major_mismatch "expected 2, found 1" awk \
		-v expected_major=2 -v expected_minor=9 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/autoCode.h"
	expectFailure autocode_version_minor_mismatch "expected 10, found 9" awk \
		-v expected_major=1 -v expected_minor=10 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/autoCode.h"
	printf '%s\n' '#define AUTOCODE_VERSION_MAJOR 1U' > "${PATH_STAGE_WORK}/missing.h"
	expectFailure autocode_version_missing "Missing autoCode version definition" awk \
		-v expected_major=1 -v expected_minor=9 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/missing.h"
	printf '%s\n' '#define AUTOCODE_VERSION_MAJOR one' \
		'#define AUTOCODE_VERSION_MINOR 9U' > "${PATH_STAGE_WORK}/malformed.h"
	expectFailure autocode_version_malformed "Invalid autoCode version definition" awk \
		-v expected_major=1 -v expected_minor=9 -f "${FILE_AUTOCODE_VERSION}" \
		"${PATH_STAGE_WORK}/malformed.h"

	printf '%s\n' \
		'addModule service first -source_file shared.c -source_dir commands' \
		'addScliCommand date -source_file shared.c' \
		'addModule task second -source_dir commands' \
		> "${PATH_STAGE_WORK}/duplicate_sources.rc"
	expectFailure initrc_duplicate_source_file "Duplicate -source_file path <srcs/shared.c>" \
		awk -v source_option=-source_file -v source_root=srcs \
		-f "${FILE_INITRC_SOURCES}" "${PATH_STAGE_WORK}/duplicate_sources.rc"
	expectFailure initrc_duplicate_source_dir "Duplicate -source_dir path <srcs/commands>" \
		awk -v source_option=-source_dir -v source_root=srcs \
		-f "${FILE_INITRC_SOURCES}" "${PATH_STAGE_WORK}/duplicate_sources.rc"

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

	PATH_GIT_CASE="${PATH_STAGE_WORK}/git_utilities"
	mkdir -p "${PATH_GIT_CASE}/repository"
	git -C "${PATH_GIT_CASE}/repository" init -q || fail "cannot initialise Git fixture"
	git -C "${PATH_GIT_CASE}/repository" -c user.name=TaskMate \
		-c user.email=test@example.invalid commit -q --allow-empty -m initial || \
		fail "cannot commit Git fixture"
	git -C "${PATH_GIT_CASE}/repository" tag v1.23 || fail "cannot tag Git fixture"
	git -C "${PATH_GIT_CASE}/repository" -c user.name=TaskMate \
		-c user.email=test@example.invalid commit -q --allow-empty -m post_tag || \
		fail "cannot add post-tag Git fixture commit"

	PATH_GIT_REPOSITORY="${PATH_GIT_CASE}/repository"
	PATH_GIT_BUNDLE="${PATH_GIT_REPOSITORY}/.backup/TaskMate.bundle_$(date '+%Y_%m_%d')"
	mkdir -p "${PATH_GIT_REPOSITORY}/scripts"
	cp "${FILE_BUNDLE}" "${PATH_GIT_REPOSITORY}/scripts/bundle.sh" || \
		fail "cannot copy bundle script into Git fixture"
	git -C "${PATH_GIT_REPOSITORY}" branch codex/delete || \
		fail "cannot create Git bundle fixture branch"
	expectSuccess bundle_create sh "${PATH_GIT_REPOSITORY}/scripts/bundle.sh"
	[ -f "${PATH_GIT_BUNDLE}" ] || fail "bundle_create: bundle file is missing"
	expectSuccess bundle_verify git -C "${PATH_GIT_REPOSITORY}" \
		bundle verify "${PATH_GIT_BUNDLE}"
	expectSuccess bundle_refs git bundle list-heads "${PATH_GIT_BUNDLE}"
	logContains bundle_refs refs/heads/codex/delete
	logContains bundle_refs refs/tags/v1.23
	expectSuccess bundle_clone git clone -q "${PATH_GIT_BUNDLE}" \
		"${PATH_GIT_CASE}/restored"
	[ "$(git -C "${PATH_GIT_CASE}/restored" rev-list --count HEAD)" -eq 2 ] || \
		fail "bundle_clone: earlier history is missing"
	[ "$(git -C "${PATH_GIT_CASE}/restored" rev-parse v1.23)" = \
		"$(git -C "${PATH_GIT_REPOSITORY}" rev-parse v1.23)" ] || \
		fail "bundle_clone: tag history is missing"

	git init -q --bare "${PATH_GIT_CASE}/remote.git" || \
		fail "cannot initialise Git remote fixture"
	git -C "${PATH_GIT_REPOSITORY}" switch -q -c test || \
		fail "cannot create test branch fixture"
	git -C "${PATH_GIT_REPOSITORY}" remote add origin "${PATH_GIT_CASE}/remote.git" || \
		fail "cannot add Git remote fixture"
	git -C "${PATH_GIT_REPOSITORY}" push -q -u origin test || \
		fail "cannot push test branch fixture"
	git -C "${PATH_GIT_REPOSITORY}" branch codex/keep || \
		fail "cannot create retained branch fixture"
	printf 'y\nn\n' > "${PATH_GIT_CASE}/branch_answers"
	expectSuccess branch_select sh -c 'cd "$1" && "$2" < "$3"' sh \
		"${PATH_GIT_REPOSITORY}" "${FILE_DEL_BRANCH}" "${PATH_GIT_CASE}/branch_answers"
	logContains branch_select "Delete local branch codex/delete? [y/N]"
	logContains branch_select "Branch kept: codex/keep"
	if git -C "${PATH_GIT_REPOSITORY}" show-ref --verify --quiet \
		refs/heads/codex/delete; then
		fail "branch_select: confirmed branch was kept"
	fi
	git -C "${PATH_GIT_REPOSITORY}" show-ref --verify --quiet \
		refs/heads/codex/keep || fail "branch_select: declined branch was deleted"

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
		system/boot.c system/sysCore/core.c system/sysCall/call.c \
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
	for FILE_CALLER in boot.c core.c call.c service.c lib.c api.h task.c
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
	printf '#include "interfaces/gpio_common.h"\n' > "${PATH_STAGE_WORK}/gpio_header.c"
	expectSuccess gpio_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -I "${PATH_GPIO_GENERATED}" -fsyntax-only \
		"${PATH_STAGE_WORK}/gpio_header.c"

	printf '#include "interfaces/hal_atomic.h"\n' > "${PATH_STAGE_WORK}/atomic_header.c"
	expectSuccess atomic_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/atomic_header.c"

	printf '#include "interfaces/hal_delay.h"\n' > "${PATH_STAGE_WORK}/delay_header.c"
	expectSuccess delay_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/delay_header.c"

	printf '%s\n' '#include "interfaces/hal_context.h"' \
		'hal_context_t *context;' 'void *stack_top;' \
		> "${PATH_STAGE_WORK}/context_header.c"
	expectSuccess context_interface_self_contained clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/context_header.c"
	printf '%s\n' '#include <stdint.h>' 'typedef uint16_t hal_stack_word_t;' \
		'#include "interfaces/hal_context.h"' \
		> "${PATH_STAGE_WORK}/context_arch_word.c"
	expectSuccess context_interface_arch_word clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/context_arch_word.c"
	printf '%s\n' '#include "hal/arch/avr8/avr8_types.h"' \
		'_Static_assert(sizeof(hal_stack_word_t) == 1U, "AVR8 stack word must be one byte");' \
		> "${PATH_STAGE_WORK}/avr8_types_header.c"
	expectSuccess avr8_types_stack_word clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/avr8_types_header.c"
	cp "${PATH_PROJECT}/srcs/hal/arch/avr8/avr8_context.inc" \
		"${PATH_STAGE_WORK}/avr8_context.inc" || fail "cannot stage avr8_context.inc"
	printf '%s\n' '#include "avr8_context.inc"' \
		'.text' '.global test_context_macros' 'test_context_macros:' \
		'AVR8_CONTEXT_SAVE' 'AVR8_CONTEXT_RESTORE' 'ret' \
		> "${PATH_STAGE_WORK}/avr8_context_macros.S"
	expectSuccess avr8_context_macros assembleAvr8ContextMacros
	cp "${PATH_PROJECT}/srcs/hal/mcu/atmega2560/at2560_timerContext.inc" \
		"${PATH_STAGE_WORK}/at2560_timerContext.inc" || \
		fail "cannot stage at2560_timerContext.inc"
	printf '%s\n' '#include "at2560_timerContext.inc"' \
		'.text' '.global test_timer_context_macros' 'test_timer_context_macros:' \
		'AT2560_TIMERCONTEXT_STOP' 'AT2560_TIMERCONTEXT_START' 'ret' \
		> "${PATH_STAGE_WORK}/at2560_timer_context_macros.S"
	expectSuccess at2560_timer_context_macros assembleAt2560TimerContextMacros
	printf '%s\n' '#include "hal/host/ucontext/ucontext_types.h"' \
		'_Static_assert(sizeof(hal_stack_word_t) == 1U, "ucontext stack word must be one byte");' \
		> "${PATH_STAGE_WORK}/ucontext_types_header.c"
	expectSuccess ucontext_types_stack_word clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/ucontext_types_header.c"

	printf '%s\n' '#include "interfaces/hal_halt.h"' \
		'_Noreturn void testHalt(void) { hal_halt(); }' \
		> "${PATH_STAGE_WORK}/halt_header.c"
	expectSuccess halt_interface_noreturn clang -std=c17 -Wall -Wextra -Werror \
		-I "${PATH_PROJECT}/srcs" -fsyntax-only "${PATH_STAGE_WORK}/halt_header.c"

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
	assertFileContains "${PATH_STAGE_WORK}/cloc.data" "build_total 23"
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

	printf '%s\n' 'unrelated_target:' '#help zebra_target: [group] Last target.' \
		'#help public_target: [group] Public target.' '_private_target:' \
		'#help alpha_target: [group] First target.' \
		> "${PATH_STAGE_WORK}/help.mk"
	printf '%s%s\n' '#help target_name_longer_than_twenty_one_characters:' \
		' [group] This description is deliberately longer than fifty-seven characters.' \
		>> "${PATH_STAGE_WORK}/help.mk"
	expectOutput help_report "alpha_target:         [group] First target.
public_target:        [group] Public target.
target_name_longer_th [group] This description is deliberately longer than fift
zebra_target:         [group] Last target." awk \
		-v COLOUR_HELP_TARGET='<target-colour>' -v COLOUR_HELP_TAG='<tag-colour>' \
		-v COLOUR_RESET='<reset-colour>' -f "${PATH_PROJECT}/scripts/make_help.awk" \
		"${PATH_STAGE_WORK}/help.mk"
	logExcludes help_report "_private_target:"
	logExcludes help_report "<target-colour>"

	expectSuccess integrated_help bmake -C "${PATH_PROJECT}" help
	logContains integrated_help "test_build_system:"
	logContains integrated_help "Run the complete build black-box test corpus."
	logContains integrated_help "TARGET=noscli:        [noscli]"
	logContains integrated_help "TARGET=test1:         [test1]"
	logContains integrated_help "TARGET=z600:          [z600]"

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
