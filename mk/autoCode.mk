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
# autoCode files and rules
################################################################################

# autoCode files
FILE_AUTOCODE_CONFIG = ${PATH_BUILD_TARGET}/autoCode_config
FILE_AUTOCODE_STAMP = ${PATH_BUILD_TARGET}/.autoCode_stamp
FILE_AUTOCODE_LOG_DATED = ${FILE_AUTOCODE_LOG}_${VAL_DATE_TIME}

FILES_AUTOCODE_INC = \
	${PATH_BUILD_GENERATED}/threads_alloc.inc \
	${PATH_BUILD_GENERATED}/drivers_alloc.inc \
	${PATH_BUILD_GENERATED}/thread_name_catalog.inc \
	${PATH_BUILD_GENERATED}/driver_name_catalog.inc \
	${PATH_BUILD_GENERATED}/error_enum.inc \
	${PATH_BUILD_GENERATED}/error_catalog.inc \
	${PATH_BUILD_GENERATED}/modules_count.inc \
	${PATH_BUILD_GENERATED}/modules_list.inc \
	${PATH_BUILD_GENERATED}/gpio_signals.inc \
	${PATH_BUILD_GENERATED}/wire_gpio.inc

CFLAGS += -I${PATH_BUILD_GENERATED}

FILES_PARSE_TAG =  \
	${PATH_SRCS}/system/sysCall/sc_errors.c \
	${PATH_SRCS}/interfaces/error_catalog.h \
	${PATH_SRCS}/interfaces/tm_modules.h \
	${PATH_SRCS}/system/sysCall/sc_threads.c \
	${PATH_SRCS}/system/sysCall/sc_driver.c \
	${PATH_SRCS}/system/sysCore/sys_modules.c \
	${PATH_SRCS}/system/sysCore/sys_modules_list.h \
	${PATH_SRCS}/interfaces/gpio_signals.h \
	${FILE_WIREGPIO_TAG}
		
FILE_INITRC_LIST = ${PATH_BUILD_TARGET}/files_initrc
FILE_PARSE_TAG_LIST = ${PATH_BUILD_TARGET}/files_to_parse
FILE_ERROR_LIST = ${PATH_BUILD_TARGET}/files_error

FILE_INITRC_DEPS = ${PATH_BUILD_TARGET}/files_initrc.deps
FILE_PARSE_TAG_DEPS = ${PATH_BUILD_TARGET}/files_to_parse.deps
FILE_ERROR_DEPS = ${PATH_BUILD_TARGET}/files_error.deps
FILE_GPIO_SIGNALS_DEPS = ${PATH_BUILD_TARGET}/gpio_signals.deps
FILE_ERROR_LEVEL = ${PATH_SRCS}/interfaces/error_level.h

# Check dynamic dependencies before evaluating the autoCode stamp.
.PHONY: _autocode
_autocode: _autocode_version_check .WAIT _autocode_dependency_check .WAIT \
	${FILE_AUTOCODE_STAMP}

# Refuse to build with an autoCode executable API version not supported by this build.
.PHONY: _autocode_version_check
_autocode_version_check: ${FILE_AUTOCODE_HEADER} ${SCRIPT_AUTOCODE_VERSION}
	@awk -v expected_major="${VAL_BUILD_AUTOCODE_EXPECTED_VER_MAJOR}" \
		-v expected_minor="${VAL_BUILD_AUTOCODE_EXPECTED_VER_MINOR}" \
		-f "${SCRIPT_AUTOCODE_VERSION}" "${FILE_AUTOCODE_HEADER}"

# autoCode launch and required files
${FILE_AUTOCODE_STAMP}: ${FILE_AUTOCODE_TARGET} ${FILE_INITRC_LIST} ${FILE_ERROR_LIST} \
						${FILE_PARSE_TAG_LIST} \
						${FILE_GPIO_SIGNALS} ${FILE_GPIO_SIGNALS_DEPS} \
						${FILE_WIREGPIO} \
						${FILES_DRIVER_INTERFACES} ${FILES_AUTOCODE_INC}

	@printf "%sautoCode, related files have changed -> run autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.if ${OPT_CLEAN_AUTOCODE_LOGS} == "yes"
	@${SCRIPT_CHECK_PATH_FILE} -d "${PATH_LOGS}"
	find "${PATH_LOGS}" -maxdepth 1 -type f -name "autoCode_log*" -delete

.endif

	# Write autoCode options
	@printf "# autoCode options\n" > "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--error_count 10" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--errors ${FILE_ERROR_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--initrc ${FILE_INITRC_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--parsetag ${FILE_PARSE_TAG_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--gpio_signals ${FILE_GPIO_SIGNALS}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--wire_gpio ${FILE_WIREGPIO}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--generated_path ${PATH_BUILD_GENERATED}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--source_path ${PATH_SRCS}" >> "${FILE_AUTOCODE_CONFIG}"
		
	# Launch autoCode
	./${FILE_AUTOCODE_TARGET} ${FILE_AUTOCODE_CONFIG} > "${FILE_AUTOCODE_LOG_DATED}"
	@touch ${FILE_AUTOCODE_STAMP}

	# Process log
	@awk ${COLOURS_AWK} -v log_file="${FILE_AUTOCODE_LOG_DATED}" \
		-f ${SCRIPT_AUTOCODE_LOG} "${FILE_AUTOCODE_LOG_DATED}"

# Missing or externally modified generated artefacts force autoCode to run.
${FILES_AUTOCODE_INC}:

# Special rule for autoCode with Clang, not the architecture-specific compiler
CFLAGS_AUTOCODE = -DAUTOCODE_BUILD -I${PATH_SRCS}/
CFLAGS_AUTOCODE += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

${FILE_AUTOCODE_TARGET}: ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H} ${FILE_ERROR_LEVEL}
	@printf "%sCompiling autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${CFLAGS_AUTOCODE} ${FILES_AUTOCODE_SRC} -o ${FILE_AUTOCODE_TARGET}

# Dependency generation
.PHONY: _autocode_dependency_check
_autocode_dependency_check:
	@${SCRIPT_COMPARE_REPLACE} \
		"${FILE_INITRC_DEPS}" "${FILES_INITRC}"
	@${SCRIPT_COMPARE_REPLACE} \
		"${FILE_PARSE_TAG_DEPS}" "${FILES_PARSE_TAG}"
	@${SCRIPT_COMPARE_REPLACE} \
		"${FILE_ERROR_DEPS}" "${FILES_ERROR}"
	@${SCRIPT_COMPARE_REPLACE} \
		"${FILE_GPIO_SIGNALS_DEPS}" "${FILE_GPIO_SIGNALS}"

# Files list for autoCode
${FILE_ERROR_LIST}: ${FILES_ERROR} ${FILE_ERROR_DEPS}
	@printf "" > ${FILE_ERROR_LIST}
.for file in ${FILES_ERROR}
	@printf "%s\n" ${file} >> ${FILE_ERROR_LIST}
.endfor

${FILE_INITRC_LIST}: ${FILES_INITRC} ${FILE_INITRC_DEPS}
	@printf "" > ${FILE_INITRC_LIST}
.for file in ${FILES_INITRC}
	@printf "%s\n" ${file} >> ${FILE_INITRC_LIST}
.endfor

${FILE_PARSE_TAG_LIST}: ${FILES_PARSE_TAG} ${FILE_PARSE_TAG_DEPS}
	@printf "" > ${FILE_PARSE_TAG_LIST}
.for file in ${FILES_PARSE_TAG}
	@printf "%s\n" ${file} >> ${FILE_PARSE_TAG_LIST}
.endfor

# Run autoCode alone
.PHONY: autoCode_alone
autoCode_alone: ${FILE_AUTOCODE_TARGET}
#help [global] Run autoCode alone.
	@printf "%sForce running autoCode alone%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@${SCRIPT_CHECK_PATH_FILE} -f "${FILE_AUTOCODE_STAMP}"
	@if [ -f "${FILE_AUTOCODE_STAMP}" ]; then \
		find "${FILE_AUTOCODE_STAMP}" -type f -delete; \
	fi
	@${MAKE} _autocode
	@ls -t ${FILE_AUTOCODE_LOG}* 2>/dev/null | head -1 | xargs cat
