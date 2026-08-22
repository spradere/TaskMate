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
# Build rules
################################################################################

# Info about help system : targets begins with '_' or '$' are internal system only
# they'll not be displayed with 'make help'

.MAIN: all

.BEGIN: ${FILE_PROGRAMS_CHECK_STAMP}
	@mkdir -p "${PATH_BUILD_TARGET}"
	@mkdir -p "${PATH_LOGS}"
	
	@printf "%s\n" "#define TM_VER_MAJOR ${VAL_TM_VER_MAJOR}" > "${FILE_TASKMATE_INFO}"
	@printf "%s\n" "#define TM_VER_MINOR ${VAL_TM_VER_MINOR}" >> "${FILE_TASKMATE_INFO}"
	@printf "%s\n" "#define TM_BUILD ${VAL_BUILD_CNT}" >> "${FILE_TASKMATE_INFO}"
	
# Check programs once on the first Make invocation in the project
${FILE_PROGRAMS_CHECK_STAMP}:
	@printf "%sChecking required programs ...%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@missing_programs=""; \
	for program in ${VAL_REQUIRED_PROGRAMS}; do \
		if ! command -v "$${program}" >/dev/null 2>&1; then \
			missing_programs="$${missing_programs} $${program}"; \
		fi; \
	done; \
	if [ -n "$${missing_programs}" ]; then \
		printf "%sMissing required programs:%s%s\n" \
			"${COLOUR_FAIL}" "$${missing_programs}" "${COLOUR_RESET}"; \
		exit 1; \
	fi
	@mkdir -p "${PATH_BUILDS}"
	@touch "${FILE_PROGRAMS_CHECK_STAMP}"

.END:
	@printf "##########################\n" > "${FILE_BUILD_INFO}"
	@printf "# Last build informations \n" >> "${FILE_BUILD_INFO}"
	@printf "##########################\n\n" >> "${FILE_BUILD_INFO}"

	@printf "TaskMate %s\n" "${VAL_TM_VERSION}" >> "${FILE_BUILD_INFO}"
	@printf "date : " >> "${FILE_BUILD_INFO}"
	@date >> "${FILE_BUILD_INFO}"
	@printf "Hardware target : %s\n" "${VAL_HW_STACK}" >> "${FILE_BUILD_INFO}"
	@printf "build counter for this target : %s\n" "${VAL_BUILD_CNT}" >> "${FILE_BUILD_INFO}"
	@git -v >> "${FILE_BUILD_INFO}"
	@printf "git tag : " >> "${FILE_BUILD_INFO}"
	@git describe --tags >> "${FILE_BUILD_INFO}"
	@printf "${CC} : " >> "${FILE_BUILD_INFO}"
	@printf "${VAL_CC_VERSION}\n" >> "${FILE_BUILD_INFO}"

.if make(upload) || make(all)
	@printf "%s\n" "${COLOUR_WHITE_BOLD}"
	@printf "##########################\n"
	@printf "# Build summary\n"
	@printf "##########################\n\n"
	@printf "\t%-16s : %s\n" "TaskMate version" "${VAL_TM_VERSION}"
	@printf "\t%-16s : %s\n" "Hardware target" "${VAL_HW_STACK}"
	@printf "\t%-16s : %s\n" "build" "${VAL_BUILD_CNT}"

	@awk -f ${PATH_SCRIPTS}/build_summary_cloc.awk "${FILE_CLOCDATA}"

	@awk ${COLOURS_AWK} -f ${PATH_SCRIPTS}/build_summary_memory.awk "${FILE_MEMDATA}"

	@printf "${COLOUR_RESET}"
.endif

all: ${FILE_GIT_IGNORE} _system_critical_check ${FILE_AUTOCODE_STAMP} _dependency \
	${FILE_TARGET} _mcu_memory_data _cloc_data
#help [global] Taskmate build.
	@printf "\n%sBuild complete%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"

# dependency files used to compile sources if related header or source was edited
_dependency:
	@if ls ${FILES_DEP} >/dev/null 2>&1; then cat ${FILES_DEP}; fi > "${FILE_DEPS_ALL}"

# autoCode and required files
${FILE_AUTOCODE_STAMP}: ${FILE_AUTOCODE_TARGET} ${FILE_INITRC_LIST} ${FILE_ERROR_LIST} \
						${FILE_PARSE_TAG_LIST} ${FILE_HALINIT_LIST} ${FILE_HALDEFINE_LIST}

	@printf "\n%sautoCode, init.rc or related files have changed -> run autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
.if ${OPT_CLEAN_AUTOCODE_LOGS} == "yes"
	@rm -f ${FILE_AUTOCODE_LOG}*
.endif

	# write autoCode options
	@printf "%s\n" "--errors ${FILE_ERROR_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--initrc ${FILE_INITRC_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--parsetag ${FILE_PARSE_TAG_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--halinit ${FILE_HALINIT_LIST}" >> "${FILE_AUTOCODE_CONFIG}"
	@printf "%s\n" "--haldefine ${FILE_HALDEFINE_LIST}" >> "${FILE_AUTOCODE_CONFIG}"	
	@printf "%s\n" "--gpio_signals ${FILE_GPIO_SIGNALS}" >> "${FILE_AUTOCODE_CONFIG}"
		
	# launch autoCode
	./${FILE_AUTOCODE_TARGET} ${FILE_AUTOCODE_CONFIG} > "${FILE_AUTOCODE_LOG_STAMP}"
	#./${FILE_AUTOCODE_TARGET} ${FILE_AUTOCODE_CONFIG}
	@touch ${FILE_AUTOCODE_STAMP}

	# proceed log
	@awk ${COLOURS_AWK} -v log_file="${FILE_AUTOCODE_LOG_STAMP}" \
		-f ${PATH_SCRIPTS}/autocode_log.awk "${FILE_AUTOCODE_LOG_STAMP}"

# Special rule for autoCode with clang, not arch specialized compiler
CFLAGS_AUTOCODE = -I${PATH_SRCS}/
CFLAGS_AUTOCODE += -Wall -Wextra -Wshadow -Wpedantic -Wconversion \
	-Wswitch -Wenum-conversion \
	-Wno-gnu-zero-variadic-macro-arguments

${FILE_AUTOCODE_TARGET}: ${FILES_AUTOCODE_SRC} ${FILES_AUTOCODE_SRC_H}
	@printf "\n%sCompiling autoCode%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	clang ${CFLAGS_AUTOCODE} ${FILES_AUTOCODE_SRC} -o ${FILE_AUTOCODE_TARGET}

# Files list for autoCode
${FILE_ERROR_LIST}: ${FILES_ERROR}
	@printf "\n%sCat all *.err files in one for autoCode%s\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@cat ${FILES_ERROR} > "${FILE_ERROR_LIST}"

${FILE_INITRC_LIST}: ${FILES_INITRC}
	@printf "" > ${FILE_INITRC_LIST}
.for file in ${FILES_INITRC}
	@printf "%s\n" ${file} >> ${FILE_INITRC_LIST}
.endfor

${FILE_PARSE_TAG_LIST}: ${FILES_PARSE_TAG}
	@printf "" > ${FILE_PARSE_TAG_LIST}
.for file in ${FILES_PARSE_TAG}
	@printf "%s\n" ${file} >> ${FILE_PARSE_TAG_LIST}
.endfor

${FILE_HALINIT_LIST}: ${FILES_HALINIT}
	@printf "" > ${FILE_HALINIT_LIST}
.for file in ${FILES_HALINIT}
	@printf "%s\n" ${file} >> ${FILE_HALINIT_LIST}
.endfor

${FILE_HALDEFINE_LIST}: ${FILES_HALDEFINE}
	@printf "" > ${FILE_HALDEFINE_LIST}
.for file in ${FILES_HALDEFINE}
	@printf "%s\n" ${file} >> ${FILE_HALDEFINE_LIST}
.endfor

# Run autoCode alone
autoCode_alone: ${FILE_AUTOCODE_TARGET}
#help [global] Run autoCode alone.
	@printf "\n%sForce running autoCode alone%s\n\n" \
		"${COLOUR_TARGET_INFO}" "${COLOUR_RESET}"
	@rm -f "${FILE_AUTOCODE_STAMP}"
	@${MAKE} ${FILE_AUTOCODE_STAMP}
	@ls -t ${FILE_AUTOCODE_LOG}* 2>/dev/null | head -1 | xargs cat
.PHONY: autoCode_alone
